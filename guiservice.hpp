//gui service
//listener from prcing will call gui service, and check if the time is ok.

#ifndef GUISERVICE_HPP
#define GUISERVICE_HPP

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time.hpp>
#include "products.hpp"
#include "pricingservice.hpp"
#include "soa.hpp"

using namespace boost::posix_time;
using namespace std;


//Connector
class GUIServiceConnector : public Connector<Price<Bond>> {
private:
	ptime timeStamp;
public:
	GUIServiceConnector() {}

	void SetTime(ptime source)
	{
		timeStamp = source;
	}

	void Publish(Price<Bond>& data)
	{
		auto bond = data.GetProduct();
		auto mid = data.GetMid();
		auto spread = data.GetBidOfferSpread();

		ofstream out;
		out.open("/home/ubuntu/final/gui.txt", ios::app);
		out << timeStamp << "," << bond.GetProductId() << "," << mid << "," << spread << endl;
	}
};


class GUIService :public Service<string, Price<Bond>> {
private:
	GUIServiceConnector* connector;
	//last time of output
	ptime last;
	time_duration throtte_time;
public:
	// constructor
	GUIService(GUIServiceConnector* source)
	{
		connector = source;
		throtte_time = millisec(300);
		last = microsec_clock::local_time();
	}

	void Throttle(Price<Bond>& data)
	{
		ptime now = microsec_clock::local_time();
		time_duration diff = now - last;

		if (diff < throtte_time) return;
		last = now;
		connector->SetTime(now);
		connector->Publish(data);
		return;
	}

	//since thie service doesn't have map and listeners, so these functions don't have to be implemented
	Price<Bond>& GetData(string id) {}

	void OnMessage(Price<Bond>& data) {}

	void AddListener(ServiceListener<Price<Bond> >* listener) {}

	const vector< ServiceListener<Price<Bond> >* >& GetListeners() const {}
};


#endif // !GUISERVICE_HPP