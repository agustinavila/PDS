Pn=150; %en HP
V=380;  %380V/660V
cosphi=.8
n=1467  %velocidad nominal
rendimiento=.82;

%% corriente nominal
disp("Punto 1 A:")
Pn=Pn*745.7;    %la convierte a Watts
In=Pn/(rendimiento*sqrt(3)*V*cosphi);
disp("la corriente nominal es "+In+"A");

%% cupla nominal y cupla de arranque
disp("Punto 1 B:")
Tn=Pn/(2*pi*n);
Tarr=.8*Tn;
disp("La cupla nominal es "+Tn+"Nm");
disp("La cupla de arranque es "+Tarr+"Nm");

%% corriente de arranque directo
disp("Punto 1 C:")
Iarr1=5*In;
Iarr2=7*In;
disp("La corriente de arranque varia entre "+Iarr1+"A y "+Iarr2+"A");

%% cupla de arranque para estrella triangulo
disp("Punto 1 E:")
Tarr_est=Tarr/3;
disp("La cupla de arranque en un arranque estrella es "+Tarr_est+"Nm");

%% deslizamiento nominal y numero de polos
disp("Punto 1 F:")
p=floor(120*f/n);   %numero de polos
ns=120*f/p;         %velocidad sincronica
s=(ns-n)/ns;        %deslizamiento
disp("El deslizamiento nominal es del "+s*100+"%");
disp("Punto 1 G:")
disp("El numero de polos es "+p);




%% punto 2
Pn=11185.5; %en W
V=220;      %220V/380V
In=22;      %en A
cosphi=.89;
sn=.055;    %deslizamiento nominal
p=8;        %cant de polos

%% rendimiento del motor
disp("Punto 2 A:");
Pabs=3*V*In*cosphi;
rendimiento=Pn/Pabs;
disp("El rendimiento es de "+rendimiento*100+"%");

%% velocidad nominal
disp("Punto 2 C:");
f=50;
ns=(120*f)/p;
disp("La velocidad nominal es de "+ns+"RPM");

%% Punto D

Tcarga=70;
transf1=.5;
transf2=.65;
transf3=.8;
%averiguar cupla de arranque

%% cupla de arranque para estrella triangulo

Tarr_est=Tarr/3;

%% deslizamiento nominal y numero de polos
p=floor(120*f/n);   %numero de polos
ns=120*f/p;         %velocidad sincronica
s=(ns-n)/ns;        %deslizamiento