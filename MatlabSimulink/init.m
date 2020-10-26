warning('off');
TbaseSim=1;

%for index = 1:NumberActuatorSensor
%    Actuator(index)=0.0;
%    Sensor(index)=0.0;
%end

CUr_Init_mmolPL=11;
CK_exInit_mmolPL=4.6;
CK_inInit_mmolPL=145;
CNa_exInit_mmolPL=145;

real_CURin=CUr_Init_mmolPL;
real_CKin=CK_inInit_mmolPL;
real_GUr=0.155; %normal=0,155pmin 74pday, overlayed by Transient(71)
real_GNa=0.000; % vermutung
real_GK=0.03125; %% 50mmol-10% = 45 mmol/day = 0,03125mmol/min
% K intake normally varies between 40 and 150 mEq/day. In the steady state, fecal losses are usually close to 10% of intake.

%aus: Potassium metabolism and gastrointestinal function; a review Spencer 1959: [0.06944]
%real_k_V=0.4;
real_V_tot=45;
real_V_ex=real_V_tot*0.375;
real_V_drink=1500;

real_GmKNa=0;


if TbaseSim>60 
    % due to production of chemical sensor Timestamp
    disp('TbaseSim should be [0.1 ..60] limiting it to 60!' );
    TbaseSim=1;
end;

if TbaseSim<0.01 
    % due to time delay in the pipes!
    disp('TbaseSim should be [0.1 ..60] limiting it to 0.1!' );
    TbaseSim=0.01;
end;

if mod(TbaseSim,0.01)~= 0
    % due to time delay in the pipes!
    disp('TbaseSim should be quantilized to 0.01 rounding it to 0.1!' );
    TbaseSim=TbaseSim*100;
    TbaseSim=round(TbaseSim);
    TbaseSim=TbaseSim/100;
end;

%defining the number of available actuators and sensor for OFFIS SimLink
%interface to OVP.

irq0VectAddr=0.0;
irq1VectAddr=0.0;
irq2VectAddr=0.0;
irq3VectAddr=0.0;

Actuator0=0.0;
Actuator1=0.0;
Actuator2=0.0;
Actuator3=0.0;
Actuator4=0.0;
Actuator5=0.0;
Actuator6=0.0;
Actuator7=0.0;
Actuator8=0.0;
Actuator9=0.0;
Actuator10=0.0;
Actuator11=0.0;
Actuator12=0.0;
Actuator13=0.0;
Actuator14=0.0;
Actuator15=0.0;
Actuator16=0.0;
Actuator17=0.0;
Actuator18=0.0;
Actuator19=0.0;
Actuator20=0.0;
Actuator21=0.0;
Actuator22=0.0;
Actuator23=0.0;
Actuator24=0.0;
Actuator25=0.0;
Actuator26=0.0;
Actuator27=0.0;
Actuator28=0.0;
Actuator29=0.0;
Actuator30=0.0;
Actuator31=0.0;
Actuator32=0.0;
Actuator33=0.0;
Actuator34=0.0;
Actuator35=0.0;
Actuator36=0.0;
Actuator37=0.0;
Actuator38=0.0;
Actuator39=0.0;
Actuator40=0.0;
Actuator41=0.0;
Actuator42=0.0;
Actuator43=0.0;
Actuator44=0.0;
Actuator45=0.0;
Actuator46=0.0;
Actuator47=0.0;
Actuator48=0.0;
Actuator49=0.0;
Actuator50=0.0;
Actuator51=0.0;
Actuator52=0.0;
Actuator53=0.0;
Actuator54=0.0;
Actuator55=0.0;
Actuator56=0.0;
Actuator57=0.0;
Actuator58=0.0;
Actuator59=0.0;
Actuator60=0.0;
Actuator61=0.0;
Actuator62=0.0;
Actuator63=0.0;
Actuator64=0.0;
Actuator65=0.0;
Actuator66=0.0;
Actuator67=0.0;
Actuator68=0.0;
Actuator69=0.0;
Actuator70=0.0;
Actuator71=0.0;
Actuator72=0.0;
Actuator73=0.0;
Actuator74=0.0;
Actuator75=0.0;
Actuator76=0.0;
Actuator77=0.0;
Actuator78=0.0;
Actuator79=0.0;
Actuator80=0.0;
Actuator81=0.0;
Actuator82=0.0;
Actuator83=0.0;
Actuator84=0.0;
Actuator85=0.0;
Actuator86=0.0;
Actuator87=0.0;
Actuator88=0.0;
Actuator89=0.0;
Actuator90=0.0;
Actuator91=0.0;
Actuator92=0.0;
Actuator93=0.0;
Actuator94=0.0;
Actuator95=0.0;
Actuator96=0.0;
Actuator97=0.0;
Actuator98=0.0;
Actuator99=0.0;

Sensor0=0.0;
Sensor1=0.0;
Sensor2=0.0;
Sensor3=0.0;
Sensor4=0.0;
Sensor5=0.0;
Sensor6=0.0;
Sensor7=0.0;
Sensor8=0.0;
Sensor9=0.0;
Sensor10=0.0;
Sensor11=0.0;
Sensor12=0.0;
Sensor13=0.0;
Sensor14=0.0;
Sensor15=0.0;
Sensor16=0.0;
Sensor17=0.0;
Sensor18=0.0;
Sensor19=0.0;
Sensor20=0.0;
Sensor21=0.0;
Sensor22=0.0;
Sensor23=0.0;
Sensor24=0.0;
Sensor25=0.0;
Sensor26=0.0;
Sensor27=0.0;
Sensor28=0.0;
Sensor29=0.0;
Sensor30=0.0;
Sensor31=0.0;
Sensor32=0.0;
Sensor33=0.0;
Sensor34=0.0;
Sensor35=0.0;
Sensor36=0.0;
Sensor37=0.0;
Sensor38=0.0;
Sensor39=0.0;
Sensor40=0.0;
Sensor41=0.0;
Sensor42=0.0;
Sensor43=0.0;
Sensor44=0.0;
Sensor45=0.0;
Sensor46=0.0;
Sensor47=0.0;
Sensor48=0.0;
Sensor49=0.0;
Sensor50=0.0;
Sensor51=0.0;
Sensor52=0.0;
Sensor53=0.0;
Sensor54=0.0;
Sensor55=0.0;
Sensor56=0.0;
Sensor57=0.0;
Sensor58=0.0;
Sensor59=0.0;
Sensor60=0.0;
Sensor61=0.0;
Sensor62=0.0;
Sensor63=0.0;
Sensor64=0.0;
Sensor65=0.0;
Sensor66=0.0;
Sensor67=0.0;
Sensor68=0.0;
Sensor69=0.0;
Sensor70=0.0;
Sensor71=0.0;
Sensor72=0.0;
Sensor73=0.0;
Sensor74=0.0;
Sensor75=0.0;
Sensor76=0.0;
Sensor77=0.0;
Sensor78=0.0;
Sensor79=0.0;
Sensor80=0.0;
Sensor81=0.0;
Sensor82=0.0;
Sensor83=0.0;
Sensor84=0.0;
Sensor85=0.0;
Sensor86=0.0;
Sensor87=0.0;
Sensor88=0.0;
Sensor89=0.0;
Sensor90=0.0;
Sensor91=0.0;
Sensor92=0.0;
Sensor93=0.0;
Sensor94=0.0;
Sensor95=0.0;
Sensor96=0.0;
Sensor97=0.0;
Sensor98=0.0;
Sensor99=0.0;