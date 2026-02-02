/*
  ==============================================================================

    ResponsiveLayout.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "ResponsiveLayout.h"

//==============================================================================
// Static breakpoint definitions
const std::vector<ResponsiveBreakpoint> ResponsiveLayout::breakpoints = {
    ResponsiveBreakpoint(0,    767,  0.75f, 14.0f, 0.75f, 0.75f, "Mobile"),
    ResponsiveBreakpoint(768,  1023, 0.9f,  16.0f, 0.85f, 0.85f, "Tablet"), 
    ResponsiveBreakpoint(1024, 1919, 1.0f,  18.0f, 1.0f,  1.0f,  "Desktop"),
    ResponsiveBreakpoint(1920, -1,   1.2f,  20.0f, 1.2f,  1.2f,  "4K")
};

//==============================================================================
// ResponsiveLayout static methods implementation

ScreenSize ResponsiveLayout::getScreenSize(int width)
{
    for (size_t i = 0; i < breakpoints.size(); ++i)
    {
        const auto& bp = breakpoints[i];
        if (width >= bp.minWidth && (bp.maxWidth == -1 || width <= bp.maxWidth))
        {
            return static_cast<ScreenSize>(i);
        }
    }
    return ScreenSize::Desktop; // Default fallback
}

ScreenSize ResponsiveLayout::getScreenSize(const Rectangle<int>& bounds)
{
    return getScreenSize(bounds.getWidth());
}

ResponsiveBreakpoint ResponsiveLayout::getBreakpoint(ScreenSize screenSize)
{
    size_t index = static_cast<size_t>(screenSize);
    if (index < breakpoints.size())
        return breakpoints[index];
    return breakpoints[2]; // Default to desktop
}

bool ResponsiveLayout::isScreenSize(int width, ScreenSize targetSize)
{
    return getScreenSize(width) == targetSize;
}

float ResponsiveLayout::getScaleFactor(ScreenSize screenSize)
{
    return getBreakpoint(screenSize).scaleFactor;
}

float ResponsiveLayout::getScaleFactor(int width)
{
    return getScaleFactor(getScreenSize(width));
}

float ResponsiveLayout::getScaleFactor(const Rectangle<int>& bounds)
{
    return getScaleFactor(bounds.getWidth());
}

float ResponsiveLayout::getAdaptiveScaleFactor(int width)
{
    // Smooth interpolation between breakpoints
    for (size_t i = 0; i < breakpoints.size() - 1; ++i)
    {
        const auto& current = breakpoints[i];
        const auto& next = breakpoints[i + 1];
        
        if (width >= current.minWidth && width < next.minWidth)
        {
            if (current.maxWidth == -1) return current.scaleFactor;
            
            float progress = (float)(width - current.minWidth) / 
                           (float)(next.minWidth - current.minWidth);
            return current.scaleFactor + progress * (next.scaleFactor - current.scaleFactor);
        }
    }
    
    return getScaleFactor(width); // Fallback to discrete scale factor
}

int ResponsiveLayout::getResponsiveWidth(int parentWidth, float percentage, ScreenSize screenSize)
{
    float scaleFactor = getScaleFactor(screenSize);
    return (int)(parentWidth * (percentage / 100.0f) * scaleFactor);
}

int ResponsiveLayout::getResponsiveHeight(int parentHeight, float percentage, ScreenSize screenSize)
{
    float scaleFactor = getScaleFactor(screenSize);
    return (int)(parentHeight * (percentage / 100.0f) * scaleFactor);
}

Rectangle<int> ResponsiveLayout::getResponsiveBounds(const Rectangle<int>& parentBounds, 
                                                   float widthPercent, float heightPercent, 
                                                   ScreenSize screenSize)
{
    int width = getResponsiveWidth(parentBounds.getWidth(), widthPercent, screenSize);
    int height = getResponsiveHeight(parentBounds.getHeight(), heightPercent, screenSize);
    
    return Rectangle<int>(parentBounds.getX(), parentBounds.getY(), width, height);
}

Rectangle<int> ResponsiveLayout::getConstrainedBounds(const Rectangle<int>& parentBounds,
                                                    float widthPercent, float heightPercent,
                                                    int minWidth, int maxWidth,
                                                    int minHeight, int maxHeight,
                                                    ScreenSize screenSize)
{
    Rectangle<int> bounds = getResponsiveBounds(parentBounds, widthPercent, heightPercent, screenSize);
    
    // Apply width constraints
    int constrainedWidth = bounds.getWidth();
    if (minWidth > 0) constrainedWidth = jmax(constrainedWidth, minWidth);
    if (maxWidth > 0) constrainedWidth = jmin(constrainedWidth, maxWidth);
    
    // Apply height constraints
    int constrainedHeight = bounds.getHeight();
    if (minHeight > 0) constrainedHeight = jmax(constrainedHeight, minHeight);
    if (maxHeight > 0) constrainedHeight = jmin(constrainedHeight, maxHeight);
    
    return bounds.withSize(constrainedWidth, constrainedHeight);
}

float ResponsiveLayout::pixelsToPercentage(int pixels, int parentDimension)
{
    if (parentDimension <= 0) return 0.0f;
    return ((float)pixels / (float)parentDimension) * 100.0f;
}

int ResponsiveLayout::percentageToPixels(float percentage, int parentDimension, ScreenSize screenSize)
{
    float scaleFactor = getScaleFactor(screenSize);
    return (int)((percentage / 100.0f) * parentDimension * scaleFactor);
}

float ResponsiveLayout::getResponsiveFontSize(float baseFontSize, ScreenSize screenSize)
{
    return baseFontSize * getBreakpoint(screenSize).scaleFactor;
}

int ResponsiveLayout::getResponsivePadding(int basePadding, ScreenSize screenSize)
{
    return (int)(basePadding * getBreakpoint(screenSize).paddingFactor);
}

int ResponsiveLayout::getResponsiveMargin(int baseMargin, ScreenSize screenSize)
{
    return (int)(baseMargin * getBreakpoint(screenSize).marginFactor);
}

int ResponsiveLayout::getResponsiveSpacing(int baseSpacing, ScreenSize screenSize)
{
    return (int)(baseSpacing * getScaleFactor(screenSize));
}

int ResponsiveLayout::getOptimalColumns(ScreenSize screenSize, int maxColumns)
{
    switch (screenSize)
    {
        case ScreenSize::Mobile:  return jmin(1, maxColumns);
        case ScreenSize::Tablet:  return jmin(2, maxColumns);
        case ScreenSize::Desktop: return jmin(3, maxColumns);
        case ScreenSize::FourK:   return jmin(4, maxColumns);
        default:                  return jmin(3, maxColumns);
    }
}

float ResponsiveLayout::getResponsiveBorderWidth(float baseBorderWidth, ScreenSize screenSize)
{
    return baseBorderWidth * getScaleFactor(screenSize);
}

Array<Rectangle<int>> ResponsiveLayout::calculateGridLayout(const Rectangle<int>& container,
                                                          int itemCount, int columns,
                                                          int spacing, ScreenSize screenSize)
{
    Array<Rectangle<int>> results;
    if (itemCount <= 0 || columns <= 0) return results;
    
    int responsiveSpacing = getResponsiveSpacing(spacing, screenSize);
    int totalSpacingX = (columns - 1) * responsiveSpacing;
    int totalSpacingY = ((itemCount - 1) / columns) * responsiveSpacing;
    
    int cellWidth = (container.getWidth() - totalSpacingX) / columns;
    int rows = (itemCount + columns - 1) / columns;
    int cellHeight = rows > 0 ? (container.getHeight() - totalSpacingY) / rows : container.getHeight();
    
    for (int i = 0; i < itemCount; ++i)
    {
        int col = i % columns;
        int row = i / columns;
        
        int x = container.getX() + col * (cellWidth + responsiveSpacing);
        int y = container.getY() + row * (cellHeight + responsiveSpacing);
        
        results.add(Rectangle<int>(x, y, cellWidth, cellHeight));
    }
    
    return results;
}

Array<Rectangle<int>> ResponsiveLayout::calculateFlexLayout(const Rectangle<int>& container,
                                                          const Array<float>& flexWeights,
                                                          bool horizontal, int spacing,
                                                          ScreenSize screenSize)
{
    Array<Rectangle<int>> results;
    if (flexWeights.isEmpty()) return results;
    
    int responsiveSpacing = getResponsiveSpacing(spacing, screenSize);
    float totalWeight = 0.0f;
    
    for (auto weight : flexWeights)
        totalWeight += weight;
    
    if (totalWeight <= 0.0f) return results;
    
    int totalSpacing = (flexWeights.size() - 1) * responsiveSpacing;
    int availableSpace = horizontal ? (container.getWidth() - totalSpacing) 
                                   : (container.getHeight() - totalSpacing);
    
    int currentPos = horizontal ? container.getX() : container.getY();
    
    for (int i = 0; i < flexWeights.size(); ++i)
    {
        float weight = flexWeights[i];
        int size = (int)((weight / totalWeight) * availableSpace);
        
        Rectangle<int> itemBounds;
        if (horizontal)
        {
            itemBounds = Rectangle<int>(currentPos, container.getY(), size, container.getHeight());
            currentPos += size + responsiveSpacing;
        }
        else
        {
            itemBounds = Rectangle<int>(container.getX(), currentPos, container.getWidth(), size);
            currentPos += size + responsiveSpacing;
        }
        
        results.add(itemBounds);
    }
    
    return results;
}

Array<Rectangle<int>> ResponsiveLayout::calculateStackLayout(const Rectangle<int>& container,
                                                           int itemCount, bool horizontal,
                                                           int spacing, ScreenSize screenSize)
{
    Array<Rectangle<int>> results;
    if (itemCount <= 0) return results;
    
    int responsiveSpacing = getResponsiveSpacing(spacing, screenSize);
    int totalSpacing = (itemCount - 1) * responsiveSpacing;
    
    if (horizontal)
    {
        int itemWidth = (container.getWidth() - totalSpacing) / itemCount;
        int currentX = container.getX();
        
        for (int i = 0; i < itemCount; ++i)
        {
            results.add(Rectangle<int>(currentX, container.getY(), itemWidth, container.getHeight()));
            currentX += itemWidth + responsiveSpacing;
        }
    }
    else
    {
        int itemHeight = (container.getHeight() - totalSpacing) / itemCount;
        int currentY = container.getY();
        
        for (int i = 0; i < itemCount; ++i)
        {
            results.add(Rectangle<int>(container.getX(), currentY, container.getWidth(), itemHeight));
            currentY += itemHeight + responsiveSpacing;
        }
    }
    
    return results;
}

//==============================================================================
// ResponsiveComponent implementation

ResponsiveComponent::ResponsiveComponent()
{
    // Initialize with default responsive behavior
    responsiveMargins = BorderSize<int>(0);
    responsivePadding = BorderSize<int>(0);
}

ResponsiveComponent::~ResponsiveComponent()
{
}

void ResponsiveComponent::resized()
{
    if (responsiveEnabled)
    {
        updateResponsiveLayout();
    }
}

void ResponsiveComponent::parentSizeChanged()
{
    if (responsiveEnabled)
    {
        triggerResponsiveUpdate();
    }
}

void ResponsiveComponent::setResponsiveEnabled(bool enabled)
{
    if (responsiveEnabled != enabled)
    {
        responsiveEnabled = enabled;
        if (enabled)
        {
            triggerResponsiveUpdate();
        }
    }
}

void ResponsiveComponent::setResponsiveSize(float widthPercent, float heightPercent)
{
    this->widthPercent = jlimit(0.0f, 100.0f, widthPercent);
    this->heightPercent = jlimit(0.0f, 100.0f, heightPercent);
    
    if (responsiveEnabled)
    {
        triggerResponsiveUpdate();
    }
}

void ResponsiveComponent::setResponsivePosition(float xPercent, float yPercent)
{
    this->xPercent = jlimit(0.0f, 100.0f, xPercent);
    this->yPercent = jlimit(0.0f, 100.0f, yPercent);
    
    if (responsiveEnabled)
    {
        triggerResponsiveUpdate();
    }
}

void ResponsiveComponent::setResponsiveSizeConstraints(int minWidth, int maxWidth, int minHeight, int maxHeight)
{
    this->minWidth = jmax(0, minWidth);
    this->maxWidth = maxWidth;
    this->minHeight = jmax(0, minHeight);
    this->maxHeight = maxHeight;
    
    if (responsiveEnabled)
    {
        triggerResponsiveUpdate();
    }
}

void ResponsiveComponent::setResponsiveMargins(int top, int right, int bottom, int left)
{
    responsiveMargins = BorderSize<int>(top, left, bottom, right);
    
    if (responsiveEnabled)
    {
        triggerResponsiveUpdate();
    }
}

void ResponsiveComponent::setResponsivePadding(int top, int right, int bottom, int left)
{
    responsivePadding = BorderSize<int>(top, left, bottom, right);
    
    if (responsiveEnabled)
    {
        triggerResponsiveUpdate();
    }
}

void ResponsiveComponent::setMaintainAspectRatio(bool maintain, float ratio)
{
    maintainAspectRatio = maintain;
    if (ratio > 0.0f)
    {
        aspectRatio = ratio;
    }
    else if (maintain)
    {
        // Calculate current aspect ratio
        Rectangle<int> currentBounds = getBounds();
        if (currentBounds.getHeight() > 0)
        {
            aspectRatio = (float)currentBounds.getWidth() / (float)currentBounds.getHeight();
        }
    }
    
    if (responsiveEnabled)
    {
        triggerResponsiveUpdate();
    }
}

ScreenSize ResponsiveComponent::getCurrentScreenSize() const
{
    Component* parent = getParentComponent();
    if (parent)
    {
        return ResponsiveLayout::getScreenSize(parent->getWidth());
    }
    return ResponsiveLayout::getScreenSize(getWidth());
}

float ResponsiveComponent::getCurrentScaleFactor() const
{
    return ResponsiveLayout::getScaleFactor(getCurrentScreenSize());
}

float ResponsiveComponent::getResponsiveFontSize(float baseFontSize) const
{
    return ResponsiveLayout::getResponsiveFontSize(baseFontSize, getCurrentScreenSize());
}

int ResponsiveComponent::getResponsiveSpacing(int baseSpacing) const
{
    return ResponsiveLayout::getResponsiveSpacing(baseSpacing, getCurrentScreenSize());
}

void ResponsiveComponent::setChildBoundsResponsive(Component* child, float xPercent, float yPercent,
                                                 float widthPercent, float heightPercent)
{
    if (child == nullptr) return;
    
    Rectangle<int> parentBounds = getLocalBounds();
    ScreenSize screenSize = getCurrentScreenSize();
    
    int x = ResponsiveLayout::percentageToPixels(xPercent, parentBounds.getWidth(), screenSize);
    int y = ResponsiveLayout::percentageToPixels(yPercent, parentBounds.getHeight(), screenSize);
    int width = ResponsiveLayout::getResponsiveWidth(parentBounds.getWidth(), widthPercent, screenSize);
    int height = ResponsiveLayout::getResponsiveHeight(parentBounds.getHeight(), heightPercent, screenSize);
    
    Rectangle<int> childBounds(x, y, width, height);
    
    if (smoothTransitions && animationDuration > 0)
    {
        animator.animateComponent(child, childBounds, 1.0f, animationDuration, false, 1.0, 0.0);
    }
    else
    {
        child->setBounds(childBounds);
    }
}

void ResponsiveComponent::layoutChildrenInGrid(const Array<Component*>& children, int columns,
                                             int spacing, bool fillHorizontally)
{
    if (children.isEmpty()) return;
    
    ScreenSize screenSize = getCurrentScreenSize();
    int optimalColumns = fillHorizontally ? columns : ResponsiveLayout::getOptimalColumns(screenSize, columns);
    
    Array<Rectangle<int>> gridBounds = ResponsiveLayout::calculateGridLayout(
        getLocalBounds(), children.size(), optimalColumns, spacing, screenSize);
    
    for (int i = 0; i < children.size() && i < gridBounds.size(); ++i)
    {
        Component* child = children[i];
        if (child != nullptr)
        {
            if (smoothTransitions && animationDuration > 0)
            {
                animator.animateComponent(child, gridBounds[i], 1.0f, animationDuration, false, 1.0, 0.0);
            }
            else
            {
                child->setBounds(gridBounds[i]);
            }
        }
    }
}

void ResponsiveComponent::layoutChildrenFlex(const Array<Component*>& children, 
                                           const Array<float>& weights,
                                           bool horizontal, int spacing)
{
    if (children.isEmpty() || weights.size() != children.size()) return;
    
    ScreenSize screenSize = getCurrentScreenSize();
    
    Array<Rectangle<int>> flexBounds = ResponsiveLayout::calculateFlexLayout(
        getLocalBounds(), weights, horizontal, spacing, screenSize);
    
    for (int i = 0; i < children.size() && i < flexBounds.size(); ++i)
    {
        Component* child = children[i];
        if (child != nullptr)
        {
            if (smoothTransitions && animationDuration > 0)
            {
                animator.animateComponent(child, flexBounds[i], 1.0f, animationDuration, false, 1.0, 0.0);
            }
            else
            {
                child->setBounds(flexBounds[i]);
            }
        }
    }
}

void ResponsiveComponent::layoutChildrenStack(const Array<Component*>& children, 
                                            bool horizontal, int spacing)
{
    if (children.isEmpty()) return;
    
    ScreenSize screenSize = getCurrentScreenSize();
    
    Array<Rectangle<int>> stackBounds = ResponsiveLayout::calculateStackLayout(
        getLocalBounds(), children.size(), horizontal, spacing, screenSize);
    
    for (int i = 0; i < children.size() && i < stackBounds.size(); ++i)
    {
        Component* child = children[i];
        if (child != nullptr)
        {
            if (smoothTransitions && animationDuration > 0)
            {
                animator.animateComponent(child, stackBounds[i], 1.0f, animationDuration, false, 1.0, 0.0);
            }
            else
            {
                child->setBounds(stackBounds[i]);
            }
        }
    }
}

void ResponsiveComponent::onScreenSizeChanged(ScreenSize oldSize, ScreenSize newSize)
{
    // Default implementation - subclasses can override
    repaint();
}

void ResponsiveComponent::onScaleFactorChanged(float newScaleFactor)
{
    // Default implementation - subclasses can override
    repaint();
}

Rectangle<int> ResponsiveComponent::calculateResponsiveBounds(const Rectangle<int>& parentBounds)
{
    ScreenSize screenSize = ResponsiveLayout::getScreenSize(parentBounds);
    
    // Calculate base bounds
    Rectangle<int> bounds = ResponsiveLayout::getConstrainedBounds(
        parentBounds, widthPercent, heightPercent, 
        minWidth, maxWidth, minHeight, maxHeight, screenSize);
    
    // Apply position
    int x = ResponsiveLayout::percentageToPixels(xPercent, parentBounds.getWidth(), screenSize);
    int y = ResponsiveLayout::percentageToPixels(yPercent, parentBounds.getHeight(), screenSize);
    bounds.setPosition(x, y);
    
    // Apply margins
    ScreenSize currentScreenSize = getCurrentScreenSize();
    BorderSize<int> scaledMargins(
        ResponsiveLayout::getResponsiveMargin(responsiveMargins.getTop(), currentScreenSize),
        ResponsiveLayout::getResponsiveMargin(responsiveMargins.getLeft(), currentScreenSize),
        ResponsiveLayout::getResponsiveMargin(responsiveMargins.getBottom(), currentScreenSize),
        ResponsiveLayout::getResponsiveMargin(responsiveMargins.getRight(), currentScreenSize)
    );
    
    bounds = scaledMargins.subtractedFrom(bounds);
    
    // Maintain aspect ratio if enabled
    if (maintainAspectRatio && aspectRatio > 0.0f)
    {
        float currentRatio = (float)bounds.getWidth() / (float)bounds.getHeight();
        
        if (currentRatio > aspectRatio)
        {
            // Too wide, reduce width
            int newWidth = (int)(bounds.getHeight() * aspectRatio);
            bounds = bounds.withWidth(newWidth);
        }
        else if (currentRatio < aspectRatio)
        {
            // Too tall, reduce height  
            int newHeight = (int)(bounds.getWidth() / aspectRatio);
            bounds = bounds.withHeight(newHeight);
        }
    }
    
    return bounds;
}

void ResponsiveComponent::updateResponsiveLayout()
{
    Component* parent = getParentComponent();
    if (parent == nullptr) return;
    
    Rectangle<int> newBounds = calculateResponsiveBounds(parent->getLocalBounds());
    
    // Check for screen size changes
    ScreenSize currentScreenSize = getCurrentScreenSize();
    float currentScaleFactor = getCurrentScaleFactor();
    
    bool screenSizeChanged = hasScreenSizeChanged();
    bool scaleFactorChanged = (abs(currentScaleFactor - lastScaleFactor) > 0.001f);
    
    if (screenSizeChanged)
    {
        onScreenSizeChanged(lastScreenSize, currentScreenSize);
        lastScreenSize = currentScreenSize;
    }
    
    if (scaleFactorChanged)
    {
        onScaleFactorChanged(currentScaleFactor);
        lastScaleFactor = currentScaleFactor;
    }
    
    // Apply new bounds
    if (smoothTransitions && animationDuration > 0 && (screenSizeChanged || scaleFactorChanged))
    {
        animator.animateComponent(this, newBounds, 1.0f, animationDuration, false, 1.0, 0.0);
    }
    else
    {
        setBounds(newBounds);
    }
}

void ResponsiveComponent::triggerResponsiveUpdate()
{
    if (responsiveEnabled)
    {
        updateResponsiveLayout();
    }
}

bool ResponsiveComponent::hasScreenSizeChanged() const
{
    return getCurrentScreenSize() != lastScreenSize;
}

//==============================================================================
// ResponsiveConstraints implementation

Rectangle<int> ResponsiveConstraints::apply(const Rectangle<int>& parentBounds, ScreenSize screenSize) const
{
    // Use screen-specific override if available
    if (hasScreenOverride(screenSize))
    {
        return getForScreen(screenSize).apply(parentBounds, screenSize);
    }
    
    // Calculate width
    int calcWidth = ResponsiveLayout::percentageToPixels(width.percentage, parentBounds.getWidth(), screenSize);
    if (width.minPixels > 0) calcWidth = jmax(calcWidth, width.minPixels);
    if (width.maxPixels > 0) calcWidth = jmin(calcWidth, width.maxPixels);
    
    // Calculate height
    int calcHeight = ResponsiveLayout::percentageToPixels(height.percentage, parentBounds.getHeight(), screenSize);
    if (height.minPixels > 0) calcHeight = jmax(calcHeight, height.minPixels);
    if (height.maxPixels > 0) calcHeight = jmin(calcHeight, height.maxPixels);
    
    // Apply aspect ratio if needed
    if (width.useAspectRatio && width.aspectRatio > 0.0f)
    {
        calcHeight = (int)(calcWidth / width.aspectRatio);
    }
    else if (height.useAspectRatio && height.aspectRatio > 0.0f)
    {
        calcWidth = (int)(calcHeight * height.aspectRatio);
    }
    
    // Calculate position
    int calcX = ResponsiveLayout::percentageToPixels(x.percentage, parentBounds.getWidth(), screenSize) + x.pixelOffset;
    int calcY = ResponsiveLayout::percentageToPixels(y.percentage, parentBounds.getHeight(), screenSize) + y.pixelOffset;
    
    // Apply center alignment
    if (x.centerAlign)
    {
        calcX = parentBounds.getX() + (parentBounds.getWidth() - calcWidth) / 2;
    }
    
    if (y.centerAlign)
    {
        calcY = parentBounds.getY() + (parentBounds.getHeight() - calcHeight) / 2;
    }
    
    Rectangle<int> result(calcX, calcY, calcWidth, calcHeight);
    
    // Apply margins
    BorderSize<int> scaledMargins(
        ResponsiveLayout::getResponsiveMargin(margins.getTop(), screenSize),
        ResponsiveLayout::getResponsiveMargin(margins.getLeft(), screenSize),
        ResponsiveLayout::getResponsiveMargin(margins.getBottom(), screenSize),
        ResponsiveLayout::getResponsiveMargin(margins.getRight(), screenSize)
    );
    
    return scaledMargins.subtractedFrom(result);
}

void ResponsiveConstraints::setScreenOverride(ScreenSize screenSize, const ResponsiveConstraints& constraints)
{
    screenOverrides[screenSize] = constraints;
}

bool ResponsiveConstraints::hasScreenOverride(ScreenSize screenSize) const
{
    return screenOverrides.find(screenSize) != screenOverrides.end();
}

ResponsiveConstraints ResponsiveConstraints::getForScreen(ScreenSize screenSize) const
{
    auto it = screenOverrides.find(screenSize);
    if (it != screenOverrides.end())
    {
        return it->second;
    }
    return *this;
}