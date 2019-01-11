#include <ndn-cxx/face.hpp>
#include <iostream>

namespace ndn {
namespace examples {

class Consumer : noncopyable
{
public:
  void
  run()
  {
    Interest interest(Name("/example/testApp/randomData"));
    interest.setInterestLifetime(2_s); // 2 seconds
    interest.setMustBeFresh(true);

    m_face.expressInterest(interest,
                           bind(&Consumer::onData1, this,  _1, _2),
                           bind(&Consumer::onNack1, this, _1, _2),
                           bind(&Consumer::onTimeout1, this, _1));

    std::cout << ">> I1: " << interest << std::endl;

    m_face.processEvents();
  }

private:
  void
  onData2(const Interest& interest, const Data& data)
  {
    std::cout << "<< D2: " << data << std::endl;
  }

  void
  onData1(const Interest& interest1, const Data& data1)
  {
    std::cout << "<< D1: " << data1 << std::endl;


    std::string handler((const char*) data1.getContent().value(), data1.getContent().value_size());
    std::cout << "Forwarder name:" << handler << std::endl;

    Interest interest2(handler);
    interest2.setInterestLifetime(2_s); // 2 seconds
    interest2.setMustBeFresh(true);

    m_face.expressInterest(interest2,
                           bind(&Consumer::onData2, this,  _1, _2),
                           bind(&Consumer::onNack2, this, _1, _2),
                           bind(&Consumer::onTimeout2, this, _1));

    std::cout << ">> I2: " << interest2 << std::endl;
  }

  void
  onNack1(const Interest& interest, const lp::Nack& nack)
  {
    std::cout << "received Nack with reason " << nack.getReason()
              << " for interest I1" << interest << std::endl;
  }

  void
  onNack2(const Interest& interest, const lp::Nack& nack)
  {
    std::cout << "received Nack with reason " << nack.getReason()
              << " for interest I2" << interest << std::endl;
  }

  void
  onTimeout1(const Interest& interest)
  {
    std::cout << "Timeout I1" << interest << std::endl;
  }

  void
  onTimeout2(const Interest& interest)
  {
    std::cout << "Timeout I2" << interest << std::endl;
  }

private:
  Face m_face;
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
