/**
 * main.cpp
 * build an instance of system and test 
 *
 * @author Breman Thuraisingham
 * @Create by Haoyu Wang
 */

#include "bondservices.hpp"
#include "listeners.hpp"
int main()
{
	//service besides historical data service
	BondAlgoExecutionService baes;
	BondAlgoStreamingService bass;
	BondExcutionService bes;
	BondInquiryService bis;
	BondMarketDataService bmds;
	BondPositionService bpositions;
	BondPricingService bprices;
	BondRiskService brs;
	BondStreamingService bss;
	BondTradeBookingService btbs;

	//gui
	GUIServiceConnector gui_connector;
	GUIService guis(&gui_connector);

	//connectors for service above
	PricingConnector price_input_connector(&bprices);
	TradeConnector trade_input_connector(&btbs);
	MarketdataConnector market_input_connector(&bmds);
	InquiryConnector inquiry_connector(&bis);
	bis.SetConnector(&inquiry_connector);
	
	//add listner
	BondTradeServiceListener trade_listener(&bpositions);
	btbs.AddListener(&trade_listener);

	BondPositionServiceListener position_listner(&brs);
	bpositions.AddListener(&position_listner);

	BondMarketDataListener market_listener(&baes);
	bmds.AddListener(&market_listener);

	AlgoExcutionListener algo_ex_listner(&bes);
	baes.AddListener(&algo_ex_listner);

	BondExcutionListener bond_ex_listner(&btbs);
	bes.AddListener(&bond_ex_listner);

	PricingServiceListener ps_listner(&bass);
	bprices.AddListener(&ps_listner);

	AlgoStreamListener algo_stream_listner(&bss);
	bass.AddListener(&algo_stream_listner);


	PricingListener price_listner(&guis);
	bprices.AddListener(&price_listner);

	//historical data service
	HistoricalDataFileConnector excution_output_connector("executions.txt","execution");
	HistoricalDataFileConnector positions_output_connector("positions.txt", "positions");
	HistoricalDataFileConnector risk_output_connector("risk.txt", "risk");
	HistoricalDataFileConnector streaming_output_connector("streaming.txt", "streaming");
	HistoricalDataFileConnector allinquiries_output_connector("allinquiries.txt", "allinquiries");

	BondHistoricalExcutionDataService bheds(&excution_output_connector);
	BondHistoricalInquiryDataService bhids(&allinquiries_output_connector);
	BondHistoricalPositionDataService bhpd(&positions_output_connector);
	BondHistoricalRiskDataService bhrs(&risk_output_connector);
	BondHistoricalStreamingDataService bhss(&streaming_output_connector);
	
	HistoricalDataServiceListener<ExecutionOrder<Bond>> his_excution_listener(&bheds);
	bes.AddListener(&his_excution_listener);
	HistoricalDataServiceListener<Inquiry<Bond>> his_inquiry_listener(&bhids);
	bis.AddListener(&his_inquiry_listener);
	HistoricalDataServiceListener<PV01<Bond>> his_risk_listener(&bhrs);
	brs.AddListener(&his_risk_listener); 
	HistoricalDataServiceListener<Position<Bond>> his_position_listener(&bhpd);
	bpositions.AddListener(&his_position_listener);
	HistoricalDataServiceListener<PriceStream<Bond>> his_stream_listener(&bhss);
	bss.AddListener(&his_stream_listener);


	price_input_connector.Subscribe();	
	trade_input_connector.Subscribe("trades.txt");
	inquiry_connector.Subscribe("inquiry.txt");
	market_input_connector.Subscribe("marketdata.txt");

	return 0;
}