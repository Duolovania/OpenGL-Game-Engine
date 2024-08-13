//#include "Audio/audiosource.h"
//
//AudioSource::AudioSource()
//{
//
//}
//
//AudioSource::~AudioSource()
//{
//    //ALCboolean closed;
//    //if (!(alcCloseDevice, closed, openALDevice, openALDevice))
//    //{
//    //    /* do we care? */
//    //}
//
//    //if (!(alcMakeContextCurrent, contextMadeCurrent, openALDevice, nullptr))
//    //{
//    //    /* what can you do? */
//    //}
//
//    //if (!(alcDestroyContext, openALDevice, openALContext))
//    //{
//    //    /* not much you can do */
//    //}
//}
//
//void AudioSource::Gen()
//{
//    ALCdevice* device = alcOpenDevice(nullptr);
//    ALCcontext* context = alcCreateContext(device, nullptr);
//    alcMakeContextCurrent(context);
//
//    ALuint buffer;
//    ALenum format;
//    ALsizei freq;
//    std::vector<char> data;
//
//    if (!LoadWAVFile("Assets/SFX/Derezz2.wav", buffer, format, freq, data)) 
//    {
//        std::cerr << "Failed to load WAV file" << std::endl;
//    }
//
//    ALuint source;
//    alGenSources(1, &source);
//    alSourcei(source, AL_BUFFER, buffer);
//    alSourcePlay(source);
//
//    // Wait for the sound to finish playing
//    ALint state;
//    do {
//        alGetSourcei(source, AL_SOURCE_STATE, &state);
//    } while (state == AL_PLAYING);
//}
//
////char* AudioSource::LoadWav(const std::string& filename, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, std::vector<char>& size)
////{
////    std::ifstream in(filename, std::ios::binary);
////    if (!in.is_open())
////    {
////        std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
////        return nullptr;
////    }
////    if (!LoadWavHeader(in, channels, sampleRate, bitsPerSample, size))
////    {
////        std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
////        return nullptr;
////    }
////
////    char* data = new char[size];
////
////    in.read(data, size);
////
////    return data;
////}
////
////bool AudioSource::LoadWavHeader(std::ifstream& file, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, std::vector<char>& size)
////{
////	char buffer[4];
////
////	if (!file.is_open())
////	{
////		return false;
////	}
////
////    // the RIFF
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read RIFF" << std::endl;
////        return false;
////    }
////    if (std::strncmp(buffer, "RIFF", 4) != 0)
////    {
////        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
////        return false;
////    }
////
////    // the size of the file
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read size of file" << std::endl;
////        return false;
////    }
////
////    // the WAVE
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read WAVE" << std::endl;
////        return false;
////    }
////    if (std::strncmp(buffer, "WAVE", 4) != 0)
////    {
////        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
////        return false;
////    }
////
////    // "fmt/0"
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read fmt/0" << std::endl;
////        return false;
////    }
////
////    // this is always 16, the size of the fmt data chunk
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read the 16" << std::endl;
////        return false;
////    }
////
////    // PCM should be 1?
////    if (!file.read(buffer, 2))
////    {
////        std::cerr << "ERROR: could not read PCM" << std::endl;
////        return false;
////    }
////
////    // the number of channels
////    if (!file.read(buffer, 2))
////    {
////        std::cerr << "ERROR: could not read number of channels" << std::endl;
////        return false;
////    }
////    channels = ConvertToInt32(buffer, 2);
////
////    // sample rate
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read sample rate" << std::endl;
////        return false;
////    }
////    sampleRate = ConvertToInt32(buffer, 4);
////
////    // (sampleRate * bitsPerSample * channels) / 8
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
////        return false;
////    }
////
////    // ?? dafaq
////    if (!file.read(buffer, 2))
////    {
////        std::cerr << "ERROR: could not read dafaq" << std::endl;
////        return false;
////    }
////
////    // bitsPerSample
////    if (!file.read(buffer, 2))
////    {
////        std::cerr << "ERROR: could not read bits per sample" << std::endl;
////        return false;
////    }
////    bitsPerSample = ConvertToInt32(buffer, 2);
////
////    // data chunk header "data"
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read data chunk header" << std::endl;
////        return false;
////    }
////    if (std::strncmp(buffer, "data", 4) != 0)
////    {
////        std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
////        return false;
////    }
////
////    // size of data
////    if (!file.read(buffer, 4))
////    {
////        std::cerr << "ERROR: could not read data size" << std::endl;
////        return false;
////    }
////    size = ConvertToInt32(buffer, 4);
////
////    /* cannot be at the end of file */
////    if (file.eof())
////    {
////        std::cerr << "ERROR: reached EOF on the file" << std::endl;
////        return false;
////    }
////    if (file.fail())
////    {
////        std::cerr << "ERROR: fail state set on the file" << std::endl;
////        return false;
////    }
////
////    return true;
////}
////
////std::int32_t AudioSource::ConvertToInt32(char* buffer, std::size_t length)
////{
////    std::int32_t a = 0;
////    if (std::endian::native == std::endian::little)
////        std::memcpy(&a, buffer, length);
////    else
////        for (std::size_t i = 0; i < length; ++i)
////            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
////    return a;
////}
//
//
//bool AudioSource::LoadWAVFile(const std::string& filename, ALuint& buffer, ALenum& format, ALsizei& freq, std::vector<char>& data) 
//{
//    std::ifstream file(filename, std::ios::binary);
//    if (!file.is_open()) {
//        std::cerr << "Failed to open WAV file: " << filename << std::endl;
//        return false;
//    }
//
//    char chunkId[4];
//    file.read(chunkId, 4); // Should be "RIFF"
//    if (strncmp(chunkId, "RIFF", 4) != 0) {
//        std::cerr << "Invalid WAV file: " << filename << std::endl;
//        return false;
//    }
//
//    file.seekg(20, std::ios::beg);
//    uint16_t audioFormat;
//    file.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));
//    if (audioFormat != 1) { // Only PCM format is supported
//        std::cerr << "Unsupported WAV format: " << audioFormat << std::endl;
//        return false;
//    }
//
//    uint16_t numChannels;
//    file.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
//
//    uint32_t sampleRate;
//    file.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
//    freq = sampleRate;
//
//    file.seekg(34, std::ios::beg);
//    uint16_t bitsPerSample;
//    file.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample));
//
//    // Determine the format
//    if (numChannels == 1) {
//        if (bitsPerSample == 8) {
//            format = AL_FORMAT_MONO8;
//        }
//        else if (bitsPerSample == 16) {
//            format = AL_FORMAT_MONO16;
//        }
//    }
//    else if (numChannels == 2) {
//        if (bitsPerSample == 8) {
//            format = AL_FORMAT_STEREO8;
//        }
//        else if (bitsPerSample == 16) {
//            format = AL_FORMAT_STEREO16;
//        }
//    }
//
//    file.seekg(40, std::ios::beg);
//    uint32_t dataSize;
//    file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
//
//    data.resize(dataSize);
//    file.read(data.data(), dataSize);
//
//    file.close();
//
//    alGenBuffers(1, &buffer);
//    alBufferData(buffer, format, data.data(), dataSize, freq);
//
//    return true;
//}