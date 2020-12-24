#ifndef LISTENERS_HPP
#define LISTENERS_HPP

#include "bondtradebookingservice.hpp"
#include "bondpositionservice.hpp"
#include "BondRiskService.hpp"
#include "bondalgostreamingservice.hpp"
#include "bondalgoexecutionservice.hpp"
#include "bondexcutionservice.hpp"
#include "bondstreamingservice.hpp"
#include"BondPricingService.hpp"
#include "guiservice.hpp"
#include "bondinquiryservice.hpp"
#include "bondhistoryservice.hpp"
//listerner registered in BondTradeBookingSerive, used for invoking bondpositionservice
class BondTradeServiceListener : public  ServiceListener<Trade<Bond>>
{
private:
	BondPositionService* bps;
public:
	BondTradeServiceListener(BondPositionService* source)
	{
		bps = source;
	}
	// Listener callback to process an add event to the Service
	void ProcessAdd(Trade<Bond>& data)
	{

	}
	// Listener callback to process a remove event to the Service
	void ProcessRemove(Trade<Bond>& data)
	{

	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Trade<Bond>& data) 
	{
		bps->AddTrade(data);
	}
};

class BondPositionServiceListener : public ServiceListener<Position<Bond>>
{
private:
	BondRiskService* brs;
public:
	BondPositionServiceListener(BondRiskService* source)
	{
		brs = source;
	}

	// Listener callback to process an add event to the Service
	void ProcessAdd(Position<Bond>& data)
	{

	}
	// Listener callback to process a remove event to the Service
	void ProcessRemove(Position<Bond>& data)
	{

	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Position<Bond>& data)
	{
		brs->AddPosition(data);
	}
};

class BondMarketDataListener : public ServiceListener<OrderBook<Bond>>
{
private:
	BondAlgoExecutionService* aeb;
public:
	BondMarketDataListener(BondAlgoExecutionService* source)
	{
		aeb = source;
	}
	// Listener callback to process an add event to the Service
	void ProcessAdd(OrderBook<Bond>& data)
	{
		aeb->SendOrder(data);
	}
	// Listener callback to process a remove event to the Service
	void ProcessRemove(OrderBook<Bond>& data)
	{

	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(OrderBook<Bond>& data)
	{

	}
};

//Algoexcution -> bondexcution
class AlgoExcutionListener : public ServiceListener<AlgoExecution<Bond>>
{
private:
	BondExcutionService* bes;
public:
	AlgoExcutionListener(BondExcutionService* source)
	{
		bes = source;
	}
	// Listener callback to process an add event to the Service
	void ProcessAdd(AlgoExecution<Bond>& data)
	{

	}
	// Listener callback to process a remove event to the Service
	void ProcessRemove(AlgoExecution<Bond>& data)
	{

	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(AlgoExecution<Bond>& data)
	{
		ExecutionOrder<Bond> eo = *data.GetOrder();
		bes->ExecuteOrder(eo, CME);
	}
};

//bondexcution -> bond trade booking service
class BondExcutionListener : public ServiceListener<ExecutionOrder<Bond>>
{
private:
	BondTradeBookingService* btbs;
	int num = 0;
	
public:
	BondExcutionListener(BondTradeBookingService* source)
	{
		btbs = source;
	}
	// Listener callback to process an add event to the Service
	void ProcessAdd(ExecutionOrder<Bond>& data)
	{
		
		Bond product = data.GetProduct();
		string tradeId = data.GetOrderId();
		double price = data.GetPrice();
		string book;
		long quantity = data.GetVisibleQuantity();
		OrderType ot = data.GetOrderType();
		Side side;
		if (ot == BID) side = SELL;
		else side = BUY;
		Trade<Bond> trade(product, tradeId, price, book, quantity, side);
		btbs->BookTrade(trade);
	}

	// Listener callback to process a remove event to the Service
	void ProcessRemove(ExecutionOrder<Bond>& data)
	{

	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(ExecutionOrder<Bond>& data)
	{
		
	}
};

class PricingServiceListener : public ServiceListener<Price<Bond>>
{
private:
	BondAlgoStreamingService* bss;
	int i = 0;
public:

	PricingServiceListener(BondAlgoStreamingService* source)
	{
		bss = source;
	}
	// Listener callback to process an add event to the Service
	void ProcessAdd(Price<Bond>& data)
	{

	}
	// Listener callback to process a remove event to the Service
	void ProcessRemove(Price<Bond>& data)
	{

	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Price<Bond>& data)
	{
		i++;
		Bond product = data.GetProduct();
		double mid = data.GetMid();
		double spread = data.GetBidOfferSpread();
		double bid = mid - spread / 2;
		double offer = mid + spread / 2;
		long visible_q;
		long hiden_q;
		if (i % 2 == 0)
		{
			visible_q = 1000000;
			hiden_q = 2000000;
		}
		else
		{
			visible_q = 2000000;
			hiden_q = 4000000;
		}
		PriceStreamOrder po_bid(bid, visible_q, hiden_q, BID);
		PriceStreamOrder po_offer(offer, visible_q, hiden_q, OFFER);
		AlgoStream<Bond> as(product, po_bid, po_offer);
		bss->OnMessage(as);
	}
};

//algostream -> bondstream
class AlgoStreamListener : public ServiceListener<AlgoStream<Bond>>
{
private:
	BondStreamingService* bss;
public:
	AlgoStreamListener(BondStreamingService* source)
	{
		bss = source;
	}

	// Listener callback to process an add event to the Service
	void ProcessAdd(AlgoStream<Bond>& data)
	{

	}
	// Listener callback to process a remove event to the Service
	void ProcessRemove(AlgoStream<Bond>& data)
	{

	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(AlgoStream<Bond>& data)
	{
		PriceStream<Bond> eo = *data.GetPriceStream();
		bss->PublishPrice(eo);
	}
};

//pricing service -> GUI service
class PricingListener : public ServiceListener<Price<Bond> > {
private:
	GUIService* gUIService;
public:
	// constructor
	PricingListener(GUIService* source)
	{
		gUIService = source;
	}

	void ProcessAdd(Price<Bond>& price)
	{
		gUIService->Throttle(price);

	}
	void ProcessRemove(Price<Bond>&)
	{

	}
	void ProcessUpdate(Price<Bond>&) 
	{

	}
};



template <typename T>
class HistoricalDataServiceListener : public ServiceListener<T>
{
public:
	HistoricalDataServiceListener(HistoricalDataService<T>* source) : service(source) {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(T& data) { service->OnMessage(data); };

	// Listener callback to process a remove event to the Service
	void ProcessRemove(T& data) {};

	// Listener callback to process an update event to the Service
	void ProcessUpdate(T& data) { service->OnMessage(data); };

private:
	HistoricalDataService<T>* service;
};
#endif // !LISTENERS_HPP
