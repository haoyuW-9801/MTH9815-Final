#ifndef BONDRISKSERVICE_HPP
#define BONDRISKSERVICE_HPP

#include "riskservice.hpp"
#include "products.hpp"

class BondRiskService : public RiskService<Bond>
{
private:
	map<string, PV01<Bond>> pv01map;
	vector< ServiceListener<PV01<Bond>>* > listeners;

public:

	// Add a position that the service will risk
	void AddPosition(Position<Bond>& position);

	// Get the bucketed risk for the bucket sector
	const PV01< BucketedSector<Bond> >& GetBucketedRisk(const BucketedSector<Bond>& sector) const;

	// Get data on our service given a key
	PV01<Bond>& GetData(string key)
	{
		return pv01map[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(PV01<Bond>& data)
	{

	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<PV01<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<PV01<Bond>>* >& GetListeners() const
	{
		return listeners;
	}
};

void BondRiskService::AddPosition(Position<Bond>& position)
{
	string product_id = (position.GetProduct()).GetProductId();
	map<string, PV01<Bond>>::iterator it;
	it = pv01map.find(product_id);
	double q = position.GetAggregatePosition();
	double pv01 = position.GetProduct().GetPV01();
	PV01<Bond> temp_pv(position.GetProduct(), pv01, q);
	if (it != pv01map.end())
	{
		pv01map[product_id] = temp_pv;

	}
	else
	{
		pv01map.insert(pair<string, PV01<Bond>>(product_id, temp_pv));
	}
	for (auto listener : listeners)
	{
		listener->ProcessAdd(pv01map[product_id]);
	}
}

const PV01< BucketedSector<Bond> >& BondRiskService::GetBucketedRisk(const BucketedSector<Bond>& sector) const
{
	double pv01 = 0.0;
	vector<Bond> products = sector.GetProducts();
	for (Bond bond : products)
	{
		auto s = bond.GetProductId();
		auto it = pv01map.find(s);
		if (it != pv01map.end())
		{
			PV01<Bond> temp_p = pv01map.at(s);
			long position = temp_p.GetQuantity();
			pv01 += position * bond.GetPV01();
		}
	}
	PV01<BucketedSector<Bond>> result(sector, pv01, 1);
	return result;
}
#endif // !BONDRISKSERVICE_HPP
