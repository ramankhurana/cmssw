#ifndef IOPool_Streamer_StreamerOutputModuleBase_h
#define IOPool_Streamer_StreamerOutputModuleBase_h

#include "FWCore/Framework/interface/OutputModule.h"
#include "IOPool/Streamer/interface/MsgTools.h"
#include "IOPool/Streamer/interface/StreamSerializer.h"
#include <memory>
#include <vector>

class InitMsgBuilder;
class EventMsgBuilder;
namespace edm {
  class ModuleCallingContext;
  class ParameterSetDescription;

  class StreamerOutputModuleBase : public OutputModule {
  public:
    explicit StreamerOutputModuleBase(ParameterSet const& ps);  
    virtual ~StreamerOutputModuleBase();
    static void fillDescription(ParameterSetDescription & desc);

  private:
    virtual void beginRun(RunPrincipal const&, ModuleCallingContext const*) override;
    virtual void endRun(RunPrincipal const&, ModuleCallingContext const*) override;
    virtual void beginJob() override;
    virtual void endJob() override;
    virtual void writeRun(RunPrincipal const&, ModuleCallingContext const*) override;
    virtual void writeLuminosityBlock(LuminosityBlockPrincipal const&, ModuleCallingContext const*) override;
    virtual void write(EventPrincipal const& e, ModuleCallingContext const*) override;

    virtual void start() const = 0;
    virtual void stop() const = 0;
    virtual void doOutputHeader(InitMsgBuilder const& init_message) const = 0;
    virtual void doOutputEvent(EventMsgBuilder const& msg) const = 0;

    std::auto_ptr<InitMsgBuilder> serializeRegistry();
    std::auto_ptr<EventMsgBuilder> serializeEvent(EventPrincipal const& e, ModuleCallingContext const* mcc); 
    void setHltMask(EventPrincipal const& e, ModuleCallingContext const*);
    void setLumiSection();

  private:
    Selections const* selections_;

    int maxEventSize_;
    bool useCompression_;
    int compressionLevel_;

    // test luminosity sections
    int lumiSectionInterval_;  
    double timeInSecSinceUTC;

    StreamSerializer serializer_;

    //Event variables, made class memebers to avoid re instatiation for each event.
    unsigned int hltsize_;
    uint32 lumi_;
    std::vector<bool> l1bit_;
    std::vector<unsigned char> hltbits_;
    uint32 origSize_;
    char host_name_[255];

    Strings hltTriggerSelections_;
    uint32 outputModuleId_;
  }; //end-of-class-def
} // end of namespace-edm

#endif
