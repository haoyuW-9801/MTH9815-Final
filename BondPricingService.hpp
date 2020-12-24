#ifndef BONDPRICINGSERVICE_HPP
#define BONDPRICINGSERVICE_HPP
#include "products.hpp"
#include "pricingservice.hpp"

class BondPricingService : public PricingService<Bond>
{
private:
	map<string, Price<Bond>> pricemap;
	vector< ServiceListener<Price<Bond>>* > listeners;
public:
	
	//default ctor
	BondPricingService()
	{
		pricemap = map<string, Price<Bond>>();
		listeners = vector< ServiceListener<Price<Bond>>* >();
	}
    

	//update the price with a given Price
	void UpdatePrice(Price<Bond>& data)
	{
		string id = data.GetProduct().GetProductId();
		map<string, Price<Bond>>::iterator it;
		it = pricemap.find(id);
		if (it != pricemap.end())
			pricemap[id] = data;
		else pricemap.insert(pair< string, Price<Bond >> (id, data));
	}

	// Get data on our service given a key
	Price<Bond>& GetData(string key)
	{
		return pricemap[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Price<Bond>& data)
	{
		this -> UpdatePrice(data);
		for (auto i : listeners)
		{
			i->ProcessUpdate(data);
		}
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<Price<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<Price<Bond>>* >& GetListeners() const
	{
		return listeners;
	}
};

class PricingConnector : public Connector<Price<Bond>>
{
private:
	BondPricingService* service;
public:

	// Connector and Destructor
	PricingConnector(BondPricingService* source)
	{
		service = source;
	}

	// get date from service first round
	void Publish(Price<Bond>& data)
	{

	}

	// Subscribe data from the file
	void Subscribe()
	{
		ifstream data("prices.txt");
		string line;
		getline(data, line);
		while (getline(data, line))
		{
			stringstream lineStream(line);
			string s1, s2, s3;
			lineStream >> s1 >> s2 >> s3 ;
			double bid = Transfrom().ConvertDecimalToPrice(s2);
			double offer = Transfrom().ConvertDecimalToPrice(s3);
			double mid = (bid + offer) / 2;
			double spread = offer - bid;
			Bond product = Bond(s1);
			Price<Bond> p(product, mid, spread);
			service->OnMessage(p);
		}
		cout << "finish price reading" << endl;
	}

};

#endif // !BONDPRICINGSERVICE_HPP



