#485 reader
rf:receive ultrasonicTag info
uart:transmit ultrasonicTag info  
2016//11/30  
发现当写入参数与ram参数一致，回复全FF信息，未作修改。    
#PIN Define  
P0.02 TX  
P0.03 RX  
P0.20 LNA_EN_PIN  
P0.19 LNA_RW_PIN  
修改485上报机制，一有障碍，触发485上报。  
需增加RSSI过滤功能
