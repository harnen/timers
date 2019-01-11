#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <iostream>

//#include "thunks.hpp"


namespace ndn {
namespace examples {

class Producer : noncopyable
{
public:
  void
  run()
  {
    m_prefixId = m_face.setInterestFilter("/handshake/producer/I1/",
                             bind(&Producer::onI1, this, _1, _2),
                             RegisterPrefixSuccessCallback(),
                             bind(&Producer::onRegisterFailed, this, _1, _2));

    m_face.processEvents();
  }

private:

  void
  onI1(const InterestFilter& filter, const Interest& i1){
    std::cout << "<< I1" << i1 << std::endl;
    sendI2();


  }

  void sendI2()
  {
    Interest i2(Name("/handshake/consumer/I2").appendNumber(m_chunkCounter++));
    i2.setInterestLifetime(2_s); // 2 seconds
    i2.setMustBeFresh(true);

    m_face.expressInterest(i2,
                           bind(&Producer::onD2, this,  _1, _2),
                           bind(&Producer::onNack, this, _1, _2),
                           bind(&Producer::onTimeout, this, _1));

    std::cout << ">> I2" << i2 << std::endl;
  }

  void
  onD2(const Interest& i2, const Data& d2)
  {
    std::cout << "<< D2: " << d2 << std::endl;

    //if there's more data, request
    if(!d2.getName().getSubName(-1, 1).isPrefixOf(Name("last"))){
      sendI2();
    //terminate the handshake if no more data is available
    }else{
      Name d1Name("/handshake/producer/I1/");
      d1Name
        .append("established") // add "testApp" component to Interest name
        .appendVersion();  // add "version" component (current UNIX timestamp in milliseconds)

      static const std::string content = "Handshake termination";
      // Create Data packet
      shared_ptr<Data> d1 = make_shared<Data>();
      d1->setName(d1Name);
      d1->setFreshnessPeriod(10_s); // 10 seconds
      d1->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());

      // Sign Data packet with default identity
      m_keyChain.sign(*d1);

      // Return Data packet to the requester
      std::cout << ">> D1: " << *d1 << std::endl;
      m_face.put(*d1);
    }
  }

  void
  onNack(const Interest& interest, const lp::Nack& nack)
  {
    std::cout << "received Nack with reason " << nack.getReason()
              << " for interest I2: " << interest << std::endl;
  }

  void
  onTimeout(const Interest& interest)
  {
    std::cout << "Timeout I1" << interest << std::endl;
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
  unsigned int m_chunkCounter = 0;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  ndn::examples::Producer producer;
  try {
    producer.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
