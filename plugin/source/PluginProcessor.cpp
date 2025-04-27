#include "JX11Book/PluginProcessor.h"
#include "JX11Book/PluginEditor.h"

namespace audio_plugin {
JX11AudioProcessor::JX11AudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ) {
}

JX11AudioProcessor::~JX11AudioProcessor() {}

const juce::String JX11AudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool JX11AudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool JX11AudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool JX11AudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double JX11AudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int JX11AudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int JX11AudioProcessor::getCurrentProgram() {
  return 0;
}

void JX11AudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String JX11AudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void JX11AudioProcessor::changeProgramName(int index,
                                                  const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void JX11AudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void JX11AudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool JX11AudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void JX11AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                      juce::MidiBuffer& midiMessages) {
  //juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (const auto metadata : midiMessages) {
    if (metadata.numBytes == 3) {
      // status byte is in metadata.data[0]
      // data bytes in [1] and [2]
    }
  }

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // the samples and the outer loop is handling the channels.
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.
  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
    auto* channelData = buffer.getWritePointer(channel);
    juce::ignoreUnused(channelData);
    // ..do something to the data...
  }
}

void JX11AudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
  int bufferOffset = 0;

  for (const auto metadata : midiMessages) {
    // Render the audio that happens before this event (if any).
    int samplesThisSegment = metadata.samplePosition - bufferOffset;
    if (samplesThisSegment > 0) {
      render(buffer, samplesThisSegment, bufferOffset);
      bufferOffset += samplesThisSegment;
    }

    // Handle the events. Ignore sysex messages.
    if (metadata.numBytes <= 3) {
      uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
      uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
      handleMIDI(metadata.data[0], data1, data2);
    }
  }

  
  // Render the audio after the last MIDI event. If there were no
  // MIDI events, this renders the entire buffer.
  int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
  if (samplesLastSegment > 0) {
    render(buffer, samplesLastSegment, bufferOffset);
  }

  midiMessages.clear();
}

bool JX11AudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JX11AudioProcessor::createEditor() {
  return new AudioPluginAudioProcessorEditor(*this);
}

void JX11AudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::ignoreUnused(destData);
}

void JX11AudioProcessor::setStateInformation(const void* data,
                                             int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  juce::ignoreUnused(data, sizeInBytes);
}
}  // namespace audio_plugin

// This creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::JX11AudioProcessor();
}
