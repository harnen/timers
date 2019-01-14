#include <ndn-cxx/face.hpp>
#include <iostream>
#include <fstream>

#define SEGMENT_SIZE 7000

using namespace std;

namespace ndn {
namespace examples {



class Consumer : noncopyable
{
public:
  void
  run()
  {
    m_file = fopen("test.bin","rb");  // r for read, b for binary
    if (m_file != NULL){
      std::cout << "File open" << std::endl;
    }else{
      cout << "Unable to open file" << std::endl;
      return;
    }
    //make sure the prefix is registered successfully
    m_prefixId = m_face.setInterestFilter("/handshake/consumer/I2",
                             bind(&Consumer::onI2, this, _1, _2),
                             //RegisterPrefixSuccessCallback(),
                             bind(&Consumer::onPrefixRegistered, this, _1),
                             bind(&Consumer::onRegisterFailed, this, _1, _2));

    m_face.processEvents();
  }


private:

  void
  onI2(const InterestFilter& filter, const Interest& i2){
    //std::cout << "<< I2" << i2 << std::endl;
    //Submit data here
    Name dataName(i2.getName());
    dataName
      .append("established") // add "testApp" component to Interest name
      .appendVersion();  // add "version" component (current UNIX timestamp in milliseconds)

    static const std::string content = "DATA TO SUBMIT";

    m_size = fread(&m_buffer, 1, sizeof(m_buffer), m_file);
    //std::cout << "Buffer size " << sizeof(m_buffer) << std::endl;
    //std::cout << "Read " << m_size << " bytes of data from the file." << std::endl;

    if(m_size != sizeof(m_buffer)){
      dataName.append("last");
    }
    // Create Data packet
    shared_ptr<Data> data = make_shared<Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(10_s); // 10 seconds
    data->setContent(reinterpret_cast<const uint8_t*>(m_buffer), m_size);

    // Sign Data packet with default identity
    m_keyChain.sign(*data);

    // Return Data packet to the requester
    //std::cout << ">> D2: " << *data << std::endl;
    m_face.put(*data);

    if(m_size != sizeof(m_buffer)){
      std::cout << "Read the whole file - exiting" << std::endl;
      fclose(m_file);
      m_face.unsetInterestFilter(m_prefixId);
    }
  }



  void
  onD1(const Interest& interest, const Data& data)
  {
    std::cout << "<< D1: " << data << std::endl;
    if(m_size != sizeof(m_buffer)){
      std::cout << "Read the whole file - exiting" << std::endl;
      fclose(m_file);
      m_face.unsetInterestFilter(m_prefixId);
    }

  }

  void
  onNack(const Interest& interest, const lp::Nack& nack)
  {
    std::cout << "received Nack with reason " << nack.getReason()
              << " for interest I1: " << interest << std::endl;
  }

  void
  onTimeout(const Interest& interest)
  {
    std::cout << "Timeout I1: " << interest << std::endl;
  }

  void onPrefixRegistered(const Name& prefix) {
    std::cout << "Prefix " << prefix << " registered." << std::endl;

    Interest interest(Name("/handshake/producer/I1"));
    interest.setInterestLifetime(2_s); // 2 seconds
    interest.setMustBeFresh(true);

    m_face.expressInterest(interest,
                           bind(&Consumer::onD1, this,  _1, _2),
                           bind(&Consumer::onNack, this, _1, _2),
                           bind(&Consumer::onTimeout, this, _1));

    std::cout << ">> I1: " << interest << std::endl;

  }

  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix \""
              << prefix << "\" in local hub's daemon (" << reason << ")"
              << std::endl;
    m_face.shutdown();
  }

private:
  Face m_face;
  KeyChain m_keyChain;
  const RegisteredPrefixId* m_prefixId;
  char m_buffer[SEGMENT_SIZE];
  FILE *m_file;
  unsigned int m_size;
};

}
}

int
main(int argc, char** argv)
{
  ndn::examples::Consumer consumer;
  try {
    consumer.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
