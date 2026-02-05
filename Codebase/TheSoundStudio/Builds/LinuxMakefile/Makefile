# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef PKG_CONFIG
  PKG_CONFIG=pkg-config
endif

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCE_DISPLAY_SPLASH_SCREEN=0" "-DTSS_SYNTHESIS_ENGINE=1" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0.0" "-DJUCE_APP_VERSION_HEX=0x1000000" $(shell $(PKG_CONFIG) --cflags alsa freetype2 fontconfig gl libcurl) -pthread -I../../JuceLibraryCode -I../../user_modules -I../../modules -I../../Source/Views/Interface/ -I../../Source/ -I../../Source/FeedbackModule/ $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := The\ Sound\ Studio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -no-pie $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs alsa freetype2 fontconfig gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCE_DISPLAY_SPLASH_SCREEN=0" "-DTSS_SYNTHESIS_ENGINE=1" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0.0" "-DJUCE_APP_VERSION_HEX=0x1000000" $(shell $(PKG_CONFIG) --cflags alsa freetype2 fontconfig gl libcurl) -pthread -I../../JuceLibraryCode -I../../user_modules -I../../modules -I../../Source/Views/Interface/ -I../../Source/ -I../../Source/FeedbackModule/ $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := The\ Sound\ Studio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -no-pie $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs alsa freetype2 fontconfig gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Build\ Configuration)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Build\ Configuration
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCE_DISPLAY_SPLASH_SCREEN=0" "-DTSS_SYNTHESIS_ENGINE=1" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=1.0.0.0" "-DJUCE_APP_VERSION_HEX=0x1000000" $(shell $(PKG_CONFIG) --cflags alsa freetype2 fontconfig gl libcurl) -pthread -I../../JuceLibraryCode -I../../user_modules -I../../modules -I../../Source/Views/Interface/ -I../../Source/ -I../../Source/FeedbackModule/ $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0" "-DJucePlugin_Build_LV2=0"
  JUCE_TARGET_APP := The\ Sound\ Studio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -no-pie $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++17 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell $(PKG_CONFIG) --libs alsa freetype2 fontconfig gl libcurl) -fvisibility=hidden -lrt -ldl -lpthread $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/SynthesisEngine_47ef4457.o \
  $(JUCE_OBJDIR)/PhysicalModelingSynth_bd23446d.o \
  $(JUCE_OBJDIR)/KarplusStrongSynth_56ce6198.o \
  $(JUCE_OBJDIR)/WavetableSynth_ad09ffd0.o \
  $(JUCE_OBJDIR)/DesignSystem_b6d4ddc9.o \
  $(JUCE_OBJDIR)/FundamentalFrequencyComponent_b23b387.o \
  $(JUCE_OBJDIR)/ChordPlayerComponent_c41ac3a9.o \
  $(JUCE_OBJDIR)/ChordPlayerSettingsComponent_8188e146.o \
  $(JUCE_OBJDIR)/CustomChordComponent_1bc2395b.o \
  $(JUCE_OBJDIR)/ShortcutComponent_ccee7538.o \
  $(JUCE_OBJDIR)/ChordScannerComponent_38a32afc.o \
  $(JUCE_OBJDIR)/FrequencyPlayerComponent_2e49a0a9.o \
  $(JUCE_OBJDIR)/FrequencyPlayerSettingsComponent_f18abe46.o \
  $(JUCE_OBJDIR)/FrequencyScannerComponent_144fedfc.o \
  $(JUCE_OBJDIR)/FeedbackModuleComponent_a8317057.o \
  $(JUCE_OBJDIR)/FeedbackModuleProcessor_c0e5cd6c.o \
  $(JUCE_OBJDIR)/FeedbackModuleParameterSettingsComponent_f0ead521.o \
  $(JUCE_OBJDIR)/FeedbackSpectrumSettingsComponent_b431476f.o \
  $(JUCE_OBJDIR)/FeedbackInputOutputComponent_9d6059f6.o \
  $(JUCE_OBJDIR)/FrequencyToLightSubComponents_744f7538.o \
  $(JUCE_OBJDIR)/FrequencyToLightComponent_92f637bf.o \
  $(JUCE_OBJDIR)/RealtimeAnalysisComponent_b59b59b7.o \
  $(JUCE_OBJDIR)/LissajousCurveAddChordComponent_5e01b61b.o \
  $(JUCE_OBJDIR)/LissajousCurveComponent_69859296.o \
  $(JUCE_OBJDIR)/SettingsComponent_119b047b.o \
  $(JUCE_OBJDIR)/PluginBrowser_Component_f91ac474.o \
  $(JUCE_OBJDIR)/PluginWindow_609dbc0c.o \
  $(JUCE_OBJDIR)/PluginRackComponent_1df4bc54.o \
  $(JUCE_OBJDIR)/2DVisualisers_325ed98b.o \
  $(JUCE_OBJDIR)/OscilloscopeVisualiserComponent_8635b878.o \
  $(JUCE_OBJDIR)/HarmonographVisualiserComponent_169b2755.o \
  $(JUCE_OBJDIR)/OctaveVisualiserComponent_84174ab7.o \
  $(JUCE_OBJDIR)/FrequencyDataComponent_c4c47080.o \
  $(JUCE_OBJDIR)/SpectrogramComponent_d8406099.o \
  $(JUCE_OBJDIR)/SpectraHarmonicsChart_69620521.o \
  $(JUCE_OBJDIR)/FrequencyColorSpectrogram_801a23e7.o \
  $(JUCE_OBJDIR)/StandingWaveSpectrogram_58605929.o \
  $(JUCE_OBJDIR)/ColourSpectrumVisualiserComponent_5b6c2c0a.o \
  $(JUCE_OBJDIR)/VisualiserContainerComponent_a3c84b7c.o \
  $(JUCE_OBJDIR)/HighQualityMeter_1dbf9115.o \
  $(JUCE_OBJDIR)/CustomRotarySlider_28821f8c.o \
  $(JUCE_OBJDIR)/WaveTableOscViewComponent_ded2fef.o \
  $(JUCE_OBJDIR)/MainViewComponent_250bec80.o \
  $(JUCE_OBJDIR)/FundamentalFrequencyProcessor_23d8109c.o \
  $(JUCE_OBJDIR)/Analyzer_8bb8c42f.o \
  $(JUCE_OBJDIR)/WaveStackOsc_1093edf9.o \
  $(JUCE_OBJDIR)/WavetableSynthProcessor_64fb476c.o \
  $(JUCE_OBJDIR)/WaveTableOsc_c6b699f3.o \
  $(JUCE_OBJDIR)/Delay_82360b64.o \
  $(JUCE_OBJDIR)/PluginAssignProcessor_73a34011.o \
  $(JUCE_OBJDIR)/FrequencyManager_b9c008ba.o \
  $(JUCE_OBJDIR)/Scales_9d1ae372.o \
  $(JUCE_OBJDIR)/ChordManager_d8642bba.o \
  $(JUCE_OBJDIR)/SynthesisLibraryManager_ffd21c87.o \
  $(JUCE_OBJDIR)/NoiseOscillator_92fbcabb.o \
  $(JUCE_OBJDIR)/PlayRepeaterBase_8ed5dfd6.o \
  $(JUCE_OBJDIR)/VotanBuffer_5805bb09.o \
  $(JUCE_OBJDIR)/OSCManager_63737e37.o \
  $(JUCE_OBJDIR)/VotanSynthProcessor_6632bc7f.o \
  $(JUCE_OBJDIR)/VAOscillator_f146a494.o \
  $(JUCE_OBJDIR)/ADSR2_1964f631.o \
  $(JUCE_OBJDIR)/ChordPlayerProcessor_dccf20be.o \
  $(JUCE_OBJDIR)/ChordScannerProcessor_51578811.o \
  $(JUCE_OBJDIR)/FrequencyPlayerProcessor_46fdfdbe.o \
  $(JUCE_OBJDIR)/FrequencyScannerProcessor_2d044b11.o \
  $(JUCE_OBJDIR)/FrequencyToLightProcessor_abaa94d4.o \
  $(JUCE_OBJDIR)/RealtimeAnalysisProcessor_ce4fb6cc.o \
  $(JUCE_OBJDIR)/LissajousCurveProcessor_8239efab.o \
  $(JUCE_OBJDIR)/PolyphaseIir2Designer_1a49c95e.o \
  $(JUCE_OBJDIR)/Oscillators_c6e6d4b4.o \
  $(JUCE_OBJDIR)/SamplerProcessor_aa8916f3.o \
  $(JUCE_OBJDIR)/SynthesisProcessor_3706f1e7.o \
  $(JUCE_OBJDIR)/CustomLookAndFeel_ab3d772e.o \
  $(JUCE_OBJDIR)/TransportToolbarComponent_d49a74ec.o \
  $(JUCE_OBJDIR)/LogFileWriter_3b6b2154.o \
  $(JUCE_OBJDIR)/ProjectManager_ec121fdd.o \
  $(JUCE_OBJDIR)/ProfileManager_fb4e274d.o \
  $(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o \
  $(JUCE_OBJDIR)/Main_90ebc5c2.o \
  $(JUCE_OBJDIR)/BinaryData_ce4232d4.o \
  $(JUCE_OBJDIR)/BinaryData2_fa243da8.o \
  $(JUCE_OBJDIR)/include_audio_fft_aadfb4f5.o \
  $(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_ara_2a4c6ef7.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_lv2_libs_12bdca08.o \
  $(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_core_CompilationTime_9257742c.o \
  $(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_dsp_aeb2060f.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_graphics_Harfbuzz_60c52ba2.o \
  $(JUCE_OBJDIR)/include_juce_graphics_Sheenbidi_c310974d.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \
  $(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o \
  $(JUCE_OBJDIR)/include_pitch_detector_af0fa345.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(JUCE_OBJDIR)/execinfo.cmd $(RESOURCES)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@$(PKG_CONFIG) --print-errors alsa freetype2 fontconfig gl libcurl
	@echo Linking "The Sound Studio - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(shell cat $(JUCE_OBJDIR)/execinfo.cmd) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/SynthesisEngine_47ef4457.o: ../../Source/SynthesisEngine.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SynthesisEngine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PhysicalModelingSynth_bd23446d.o: ../../Source/PhysicalModelingSynth.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PhysicalModelingSynth.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KarplusStrongSynth_56ce6198.o: ../../Source/KarplusStrongSynth.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling KarplusStrongSynth.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WavetableSynth_ad09ffd0.o: ../../Source/WavetableSynth.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WavetableSynth.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DesignSystem_b6d4ddc9.o: ../../Source/UI/DesignSystem.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling DesignSystem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FundamentalFrequencyComponent_b23b387.o: ../../Source/FundamentalFrequencyComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FundamentalFrequencyComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordPlayerComponent_c41ac3a9.o: ../../Source/ChordPlayerComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChordPlayerComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordPlayerSettingsComponent_8188e146.o: ../../Source/ChordPlayerSettingsComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChordPlayerSettingsComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CustomChordComponent_1bc2395b.o: ../../Source/CustomChordComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CustomChordComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShortcutComponent_ccee7538.o: ../../Source/ShortcutComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ShortcutComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordScannerComponent_38a32afc.o: ../../Source/ChordScannerComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChordScannerComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyPlayerComponent_2e49a0a9.o: ../../Source/FrequencyPlayerComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyPlayerComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyPlayerSettingsComponent_f18abe46.o: ../../Source/FrequencyPlayerSettingsComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyPlayerSettingsComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyScannerComponent_144fedfc.o: ../../Source/FrequencyScannerComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyScannerComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FeedbackModuleComponent_a8317057.o: ../../Source/FeedbackModule/FeedbackModuleComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FeedbackModuleComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FeedbackModuleProcessor_c0e5cd6c.o: ../../Source/FeedbackModule/FeedbackModuleProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FeedbackModuleProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FeedbackModuleParameterSettingsComponent_f0ead521.o: ../../Source/FeedbackModule/FeedbackModuleParameterSettingsComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FeedbackModuleParameterSettingsComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FeedbackSpectrumSettingsComponent_b431476f.o: ../../Source/FeedbackModule/FeedbackSpectrumSettingsComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FeedbackSpectrumSettingsComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FeedbackInputOutputComponent_9d6059f6.o: ../../Source/FeedbackModule/FeedbackInputOutputComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FeedbackInputOutputComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyToLightSubComponents_744f7538.o: ../../Source/FrequencyToLightSubComponents.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyToLightSubComponents.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyToLightComponent_92f637bf.o: ../../Source/FrequencyToLightComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyToLightComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RealtimeAnalysisComponent_b59b59b7.o: ../../Source/RealtimeAnalysisComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling RealtimeAnalysisComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LissajousCurveAddChordComponent_5e01b61b.o: ../../Source/LissajousCurveAddChordComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LissajousCurveAddChordComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LissajousCurveComponent_69859296.o: ../../Source/LissajousCurveComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LissajousCurveComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsComponent_119b047b.o: ../../Source/SettingsComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SettingsComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginBrowser_Component_f91ac474.o: ../../Source/PluginBrowser_Component.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PluginBrowser_Component.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginWindow_609dbc0c.o: ../../Source/PluginWindow.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PluginWindow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginRackComponent_1df4bc54.o: ../../Source/PluginRackComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PluginRackComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/2DVisualisers_325ed98b.o: ../../Source/2DVisualisers.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling 2DVisualisers.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OscilloscopeVisualiserComponent_8635b878.o: ../../Source/OscilloscopeVisualiserComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling OscilloscopeVisualiserComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HarmonographVisualiserComponent_169b2755.o: ../../Source/HarmonographVisualiserComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling HarmonographVisualiserComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OctaveVisualiserComponent_84174ab7.o: ../../Source/OctaveVisualiserComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling OctaveVisualiserComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyDataComponent_c4c47080.o: ../../Source/FrequencyDataComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyDataComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpectrogramComponent_d8406099.o: ../../Source/SpectrogramComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SpectrogramComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpectraHarmonicsChart_69620521.o: ../../Source/SpectraHarmonicsChart.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SpectraHarmonicsChart.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyColorSpectrogram_801a23e7.o: ../../Source/FrequencyColorSpectrogram.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyColorSpectrogram.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/StandingWaveSpectrogram_58605929.o: ../../Source/StandingWaveSpectrogram.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling StandingWaveSpectrogram.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColourSpectrumVisualiserComponent_5b6c2c0a.o: ../../Source/ColourSpectrumVisualiserComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ColourSpectrumVisualiserComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VisualiserContainerComponent_a3c84b7c.o: ../../Source/VisualiserContainerComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VisualiserContainerComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HighQualityMeter_1dbf9115.o: ../../Source/HighQualityMeter.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling HighQualityMeter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CustomRotarySlider_28821f8c.o: ../../Source/CustomRotarySlider.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CustomRotarySlider.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WaveTableOscViewComponent_ded2fef.o: ../../Source/WaveTableOscViewComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WaveTableOscViewComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainViewComponent_250bec80.o: ../../Source/MainViewComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MainViewComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FundamentalFrequencyProcessor_23d8109c.o: ../../Source/FundamentalFrequencyProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FundamentalFrequencyProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Analyzer_8bb8c42f.o: ../../Source/Analyzer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Analyzer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WaveStackOsc_1093edf9.o: ../../Source/WaveStackOsc.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WaveStackOsc.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WavetableSynthProcessor_64fb476c.o: ../../Source/WavetableSynthProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WavetableSynthProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WaveTableOsc_c6b699f3.o: ../../Source/WaveTableOsc.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling WaveTableOsc.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Delay_82360b64.o: ../../Source/Delay.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Delay.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginAssignProcessor_73a34011.o: ../../Source/PluginAssignProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PluginAssignProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyManager_b9c008ba.o: ../../Source/FrequencyManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Scales_9d1ae372.o: ../../Source/Scales.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Scales.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordManager_d8642bba.o: ../../Source/ChordManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChordManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SynthesisLibraryManager_ffd21c87.o: ../../Source/SynthesisLibraryManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SynthesisLibraryManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoiseOscillator_92fbcabb.o: ../../Source/NoiseOscillator.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling NoiseOscillator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PlayRepeaterBase_8ed5dfd6.o: ../../Source/PlayRepeaterBase.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PlayRepeaterBase.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VotanBuffer_5805bb09.o: ../../Source/VotanBuffer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VotanBuffer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OSCManager_63737e37.o: ../../Source/OSCManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling OSCManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VotanSynthProcessor_6632bc7f.o: ../../Source/VotanSynthProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VotanSynthProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VAOscillator_f146a494.o: ../../Source/VAOscillator.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling VAOscillator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ADSR2_1964f631.o: ../../Source/ADSR2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ADSR2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordPlayerProcessor_dccf20be.o: ../../Source/ChordPlayerProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChordPlayerProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordScannerProcessor_51578811.o: ../../Source/ChordScannerProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ChordScannerProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyPlayerProcessor_46fdfdbe.o: ../../Source/FrequencyPlayerProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyPlayerProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyScannerProcessor_2d044b11.o: ../../Source/FrequencyScannerProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyScannerProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FrequencyToLightProcessor_abaa94d4.o: ../../Source/FrequencyToLightProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling FrequencyToLightProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RealtimeAnalysisProcessor_ce4fb6cc.o: ../../Source/RealtimeAnalysisProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling RealtimeAnalysisProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LissajousCurveProcessor_8239efab.o: ../../Source/LissajousCurveProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LissajousCurveProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PolyphaseIir2Designer_1a49c95e.o: ../../Source/hiir/PolyphaseIir2Designer.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling PolyphaseIir2Designer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Oscillators_c6e6d4b4.o: ../../Source/Oscillators.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Oscillators.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SamplerProcessor_aa8916f3.o: ../../Source/SamplerProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SamplerProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SynthesisProcessor_3706f1e7.o: ../../Source/SynthesisProcessor.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling SynthesisProcessor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CustomLookAndFeel_ab3d772e.o: ../../Source/CustomLookAndFeel.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling CustomLookAndFeel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TransportToolbarComponent_d49a74ec.o: ../../Source/TransportToolbarComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling TransportToolbarComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogFileWriter_3b6b2154.o: ../../Source/LogFileWriter.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling LogFileWriter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectManager_ec121fdd.o: ../../Source/ProjectManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ProjectManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProfileManager_fb4e274d.o: ../../Source/ProfileManager.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling ProfileManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o: ../../Source/MainComponent.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling MainComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData2_fa243da8.o: ../../JuceLibraryCode/BinaryData2.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling BinaryData2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_audio_fft_aadfb4f5.o: ../../JuceLibraryCode/include_audio_fft.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_audio_fft.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o: ../../JuceLibraryCode/include_juce_audio_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o: ../../JuceLibraryCode/include_juce_audio_devices.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_devices.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o: ../../JuceLibraryCode/include_juce_audio_formats.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_formats.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o: ../../JuceLibraryCode/include_juce_audio_processors.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_ara_2a4c6ef7.o: ../../JuceLibraryCode/include_juce_audio_processors_ara.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors_ara.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_lv2_libs_12bdca08.o: ../../JuceLibraryCode/include_juce_audio_processors_lv2_libs.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_processors_lv2_libs.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o: ../../JuceLibraryCode/include_juce_audio_utils.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_audio_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_CompilationTime_9257742c.o: ../../JuceLibraryCode/include_juce_core_CompilationTime.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_core_CompilationTime.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o: ../../JuceLibraryCode/include_juce_cryptography.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_cryptography.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_dsp_aeb2060f.o: ../../JuceLibraryCode/include_juce_dsp.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_dsp.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_Harfbuzz_60c52ba2.o: ../../JuceLibraryCode/include_juce_graphics_Harfbuzz.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_Harfbuzz.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_Sheenbidi_c310974d.o: ../../JuceLibraryCode/include_juce_graphics_Sheenbidi.c
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_graphics_Sheenbidi.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o: ../../JuceLibraryCode/include_juce_opengl.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_juce_opengl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_pitch_detector_af0fa345.o: ../../JuceLibraryCode/include_pitch_detector.cpp
	-$(V_AT)mkdir -p $(@D)
	@echo "Compiling include_pitch_detector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/execinfo.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link libexecinfo"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/execinfo.x -lexecinfo - >/dev/null 2>&1 && printf -- "-lexecinfo" > "$@" || touch "$@"

$(JUCE_OBJDIR)/cxxfs.cmd:
	-$(V_AT)mkdir -p $(@D)
	-@if [ -z "$(V_AT)" ]; then echo "Checking if we need to link stdc++fs"; fi
	$(V_AT)printf "int main() { return 0; }" | $(CXX) -x c++ -o $(@D)/cxxfs.x -lstdc++fs - >/dev/null 2>&1 && printf -- "-lstdc++fs" > "$@" || touch "$@"

clean:
	@echo Cleaning The Sound Studio
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping The Sound Studio
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

-include $(OBJECTS_APP:%.o=%.d)
