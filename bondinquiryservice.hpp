/**
 * bondinquiryService.hpp
 * Definition of bond inquiry service class
 *
 * @author Breman Thuraisingham
 * @Create by Haoyu Wang
 */

#ifndef BONDINQUIRYSERVICE_HPP
#define BONDINQUIRYSERVICE_HPP

#include "inquiryservice.hpp"
#include "products.hpp"

//pre declare
class InquiryConnector;

//bond inquiry service
class BondInquiryService :public InquiryService<Bond>
{
private:
	vector< ServiceListener<Inquiry<Bond>>* > listeners;
	map<string, Inquiry<Bond>> inquiry_map;
	InquiryConnector* connector;

public:
	void SetConnector(InquiryConnector* source)
	{
		connector = source;
	}

	// Send a quote back to the client
	void SendQuote(const string& inquiryId, double price)
	{
		Inquiry<Bond>& inquiry = inquiry_map[inquiryId];
		inquiry.SetPrice(price);
		for (auto listener : listeners)
		{
			listener->ProcessAdd(inquiry);
		}
	}

	// Reject an inquiry from the client
	void RejectInquiry(const string& inquiryId)
	{
		Inquiry<Bond> inquiry = inquiry_map[inquiryId];
		inquiry.SetState(REJECTED);
		if (inquiry_map.find(inquiryId) != inquiry_map.end())
			inquiry_map[inquiryId] = inquiry;
		else inquiry_map.insert(pair<string, Inquiry<Bond>>(inquiryId, inquiry));
	}

	// Get data on our service given a key
	Inquiry<Bond>& GetData(string key)
	{
		return inquiry_map[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Inquiry<Bond>& data);

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<Inquiry<Bond>>* listener)
	{
		listeners.push_back(listener);
	}
	// Get all listeners on the Service.
	const vector< ServiceListener<Inquiry<Bond>>* >& GetListeners() const
	{
		return listeners;
	}
};

class InquiryConnector : public Connector<Inquiry<Bond>>
{
private:
	InquiryService<Bond>* service;
public:

	// Connector and Destructor
	InquiryConnector(InquiryService<Bond>* source)
	{
		service = source;
	}

	// get date from service first round
	void Publish(Inquiry<Bond>& data)
	{
		InquiryState state = data.GetState();
		if (state == RECEIVED)
		{
			data.SetState(QUOTED);
			Subscribe(data);
		}
	}

	// Subscribe data from the file
	void Subscribe(string filename)
	{
		ifstream data;
		data.open(filename.data());
		string line;
		getline(data, line);
		while (getline(data, line))
		{
			stringstream lineStream(line);
			string s1, s2, s3, s4, s5, s6;
			lineStream >> s1 >> s2 >> s3 >> s4 >> s5 >> s6;
			string inquiryId = s1;
			string productId = s2;
			Side side;
			if (s3 == "BUY") side = BUY;
			else if (s3 == "SELL") side = SELL;
			long quantity = stol(s4);
			double price = Transfrom().ConvertDecimalToPrice(s5);
			InquiryState state;
			if (s6 == "RECEIVED") state = RECEIVED;
			else if (s6 == "QUOTED") state = QUOTED;
			else if (s6 == "DONE") state = DONE;
			else if (s6 == "REJECTED") state = REJECTED;
			else if (s6 == "CUSTOMER_REJECTED") state = CUSTOMER_REJECTED;
			Bond product = Bond(productId);
			Inquiry<Bond> inquiry(inquiryId, product, side, quantity, price, state);
			service->OnMessage(inquiry);
		}
		cout << "finish inquiry data reading" << endl;
	}
	//subscribe from the listener(2nd)
	void Subscribe(Inquiry<Bond>& data)
	{
		service->OnMessage(data);
	}

};

void BondInquiryService::OnMessage(Inquiry<Bond>& data)
{
	InquiryState state = data.GetState();
	string inquiryId = data.GetInquiryId();
	switch (state)
	{
	case RECEIVED:
		if (inquiry_map.find(inquiryId) != inquiry_map.end())
			inquiry_map[inquiryId] = data;
		else inquiry_map.insert(pair<string, Inquiry<Bond>>(inquiryId, data));		
		connector->Publish(data);
		break;
	case QUOTED:
		data.SetState(DONE);
		if (inquiry_map.find(inquiryId) != inquiry_map.end())
			inquiry_map[inquiryId] = data;
		else inquiry_map.insert(pair<string, Inquiry<Bond>>(inquiryId, data));
		for (auto listener : listeners)
		{
			listener->ProcessAdd(data);
		}

		break;
	}
}
#endif // !BONDINQUIRYSERVICE_HPP
