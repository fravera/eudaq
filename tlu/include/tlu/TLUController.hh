#ifndef H_TLUController_hh
#define H_TLUController_hh

#include "ZestSC1.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "eudaq/Utils.hh"

namespace tlu {

  static const int TLU_TRIGGER_INPUTS = 4;
  double Timestamp2Seconds(unsigned long long t);

  class TLUException : public std::runtime_error {
  public:
    TLUException(const std::string & msg, int status = 0, int tries = 0)
      : std::runtime_error(make_msg(msg, status, tries).c_str()),
	m_status(status),
	m_tries(tries)
    {}
    int GetStatus() const { return m_status; }
    int GetTries() const { return m_tries; }
  private:
    static std::string make_msg(const std::string & msg, int status, int tries);
    int m_status, m_tries;
  };

  class TLUEntry {
  public:
    TLUEntry(unsigned long long t = 0, unsigned long e = 0)
      : m_timestamp(t), m_eventnum(e) {}
    unsigned long long Timestamp() const { return m_timestamp; }
    unsigned long Eventnum() const { return m_eventnum; }
    void Print(std::ostream & out = std::cout) const;
  private:
    unsigned long long m_timestamp;
    unsigned long m_eventnum;
  };

  class TLUController {
  public:
    //typedef void (*ErrorHandler)(const char * function,
    //                             ZESTSC1_HANDLE,
    //                             ZESTSC1_STATUS,
    //                             const char * msg);
    enum ErrorHandler { // What to do if a usb access returns an error
      ERR_ABORT,        // Abort the program (used for debugging)
      ERR_THROW,        // Throw a TLUException
      ERR_RETRY1,       // Retry once before throwing
      ERR_RETRY2        // Retry twice
    };

    TLUController(const std::string & filename = "", int errormech = ERR_RETRY1);
    ~TLUController();

    //void SetFilename(const std::string & filename) { m_filename = filename; }
    void SetDUTMask(unsigned char mask);
    void SetVetoMask(unsigned char mask);
    void SetAndMask(unsigned char mask);
    void SetOrMask(unsigned char mask);
    unsigned char GetVetoMask() const;
    unsigned char GetAndMask() const;
    unsigned char GetOrMask() const;

    void SetTriggerInterval(unsigned millis);

    //void Configure();
    void Update();
    void Start();
    void Stop();
    void ResetTriggerCounter();
    void ResetScalers();
    void ResetUSB();

    size_t NumEntries() const { return m_buffer.size(); }
    TLUEntry GetEntry(size_t i) const { return m_buffer[i]; }
    unsigned GetTriggerNum() const { return m_triggernum; }
    unsigned long long GetTimestamp() const { return m_timestamp; }

    unsigned char GetTriggerStatus() const ;

    void InhibitTriggers(bool inhibit = true);

    void Print(std::ostream & out = std::cout) const;

    unsigned GetFirmwareID() const;
    static unsigned GetLibraryID();
    void SetLEDs(unsigned);
    unsigned GetLEDs() const;

    unsigned GetScaler(unsigned) const;
  private:
    static ZESTSC1_HANDLE OpenTLU();
    void Initialize();
    void WriteRegister(unsigned long offset, unsigned char val);
    unsigned char ReadRegister8(unsigned long offset) const;
    unsigned short ReadRegister16(unsigned long offset) const;
    unsigned long ReadRegister32(unsigned long offset) const;
    unsigned long long ReadRegister64(unsigned long offset) const;
    unsigned long long * ReadBlock(unsigned entries);
    unsigned char ReadRegisterRaw(unsigned long offset) const;

    std::string m_filename;
    ZESTSC1_HANDLE m_handle;
    unsigned char m_mask, m_vmask, m_amask, m_omask;
    unsigned m_triggerint;
    bool m_inhibit;

    unsigned m_vetostatus, m_fsmstatus, m_ledstatus;
    unsigned m_triggernum;
    unsigned long long m_timestamp;
    std::vector<TLUEntry> m_buffer;
    unsigned long long * m_oldbuf;
    unsigned m_scalers[TLU_TRIGGER_INPUTS];
    mutable unsigned long long m_lasttime;
    int m_errorhandler;
  };

  inline std::ostream & operator << (std::ostream & o, const TLUController & t) {
    t.Print(o);
    return o;
  }

  inline std::ostream & operator << (std::ostream & o, const TLUEntry & t) {
    t.Print(o);
    return o;
  }

}

#endif
