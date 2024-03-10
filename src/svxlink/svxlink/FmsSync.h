#ifndef FmsSync_INCLUDED
#define FmsSync_INCLUDED

#include <AsyncAudioSink.h>
#include <sigc++/sigc++.h>

class FmsSync : public Async::AudioSink, public sigc::trackable {
  public:
    FmsSync(unsigned baudrate, unsigned sample_rate=INTERNAL_SAMPLE_RATE);
    ~FmsSync(void);
    char fms_is_crc_correct(uint64_t message);
    int writeSamples(const float *samples, int len);
    void flushSamples(void);

    sigc::signal<void, uint64_t&> packetReceived;

  private:
    const unsigned    baudrate;
    const unsigned    sample_rate;
    const unsigned    shift_pos;
    unsigned          pos;
    bool              was_mark;

    uint32_t rxstate;
    uint64_t rxbitstream; // holds RXed bits
    uint32_t rxbitcount; // counts RXed bits

    FmsSync(const FmsSync&);
    FmsSync& operator=(const FmsSync&);
};
#endif
