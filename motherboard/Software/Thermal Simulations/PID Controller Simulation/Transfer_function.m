%% System parameters
% General
Tenv = -50 + 273.15;% Ambient temperature [K]
T0 = 27.5 + 273.15; % Nominal temperature [K]
DeltaT = 5;         % Temperature difference in operating range [K]
m = 0.05;           % Mass of the object [kg]

% Geometry
L = 0.1;                    % Length of tube [m]
D = 0.03;                   % Diameter of the tube [m]
r1 = D / 2;                 % Radius of the tube [m]
thickness = 0.003;          % Thickness of the glass tube [m]
L_box = 0.39;               % Dimension stzrofoam box [m]
r2 = 0.12;                  % Radius of the air area [m]
r3 = sqrt(L_box^2/pi - r2^2);% Adjusted Radius of the styrofoam outside area [m]
A_axial = pi * r1^2;        % Area of the tube at the end [m²]
A_radial = 2 * pi * r1 * L; % Area of the cylinder tube [m²]
volume_water = 50;          % Volume of water [mL]

% Thermal properties
d_quartz_plate = 0.004;     % Thickness of quartz glass plate [m]
d_air_gap = 0.004;          % Thickness of air gap [m]
d_quartz_tube = 0.002;      % Thickness of quartz glass tube [m]
k_quartz = 1.4;             % Thermal conductivity of quartz [W/m·K]
k_air = 0.015;              % Adjusted thermal conductivity of air [W/m·K]
k_styrofoam = 0.025;        % Thermal conductivity of styrofoam [W/m·K]
e = 0.66;                   % Emissivity of quartz glass [-]
sigma = 5.67*10^(-8);       % Steffan Boltzmann constant [W/m²K⁴]

% Water properties
c_p = 4181;         % Specific heat capacity [J/(kg·K)]
beta = 2.07e-4;     % Thermal expansion coefficient [1/K]
nu = 1.0e-6;        % Kinematic viscosity [m²/s]
alpha = 1.43e-7;    % Thermal diffusivity [m²/s]
k_water = 0.69;     % Thermal conductivity of water [W/m·K]
h_air = 0.254;      % Convective heat transfer coefficient of air [W/m²K⁴]

%% Convection calculations

% Water convection
% Calculate Rayleigh Number
Ra = (9.81 * beta * DeltaT * (L/2)^3) / (nu * alpha);  % Rayleigh number

% Calculate Nusselt Number
% Assuming typical empirical constants for a vertical tube: C = 0.1, n = 1/3, m = 0
C = 0.1;
n = 1/3;
Nu = C * Ra^n;  % Nusselt number

% Convective Heat Transfer Coefficient
h_water = (Nu * k_water) / (L/2);  % Convective heat transfer coefficient  of water [W/m²K]

% Velocity of Convection Currents
v = (h_water * DeltaT) / (m * c_p);  % Velocity of convection currents [m/s]

% Time Delay
tau = L/2 / v;  % Time delay [s]


%% Conduction calculations

% Thermal resistances axial
R_quartz_plate = d_quartz_plate / (k_quartz * A_axial);     % Thermal resistance of quartz plate [K/W]
R_air_gap = d_air_gap / (k_air * A_axial);                  % Thermal resistance of air gap [K/W]
R_quartz_tube = d_quartz_tube / (k_quartz * A_axial);       % Thermal resistance of quartz tube [K/W]
R1 = R_quartz_plate + R_air_gap + R_quartz_tube;            % Total thermal resistance axial [K/W]


% Thermal resistances radial
R_quartz_cyl =  (log((r1+thickness)/r1))/(2 * pi * L * k_quartz);   % Thermal resistance of quartz tube [K/W]
R_air_cyl = 1 / (A_radial * h_air);                                 % Thermal resistance of air gap [K/W]
R_styrofoam = (log((r3)/r2))/(2 * pi * L * k_styrofoam);            % Thermal resistance of styrofoam [K/W]
R2 = R_quartz_cyl + R_air_cyl + R_styrofoam;                        % Total thermal resistance radial [K/W]


%% Radiation calculations

c = e * sigma * A_radial; % Radiative transfer coefficient axial glass [W/K⁴]

%% Coefficients computation
% Calculate the coefficient K
K = (1/R1) + (1/R2) + 4 * c * T0^3;

% Create the transfer function
G = tf(1, [m*c_p, K]);

%% Delay included 
% Define the time delay as exp(-tau·s) in the numerator
delay = tau;  % Delay in seconds

% Apply the Pade approximation for the delay (first-order approximation)
[pade_num, pade_den] = pade(delay, 1);
G_delayed = G * tf(pade_num, pade_den);

% Display the transfer function with delay
disp('The transfer function of the system with delay is:')
disp(G_delayed)


% Get time vector and response
[y, t] = step(G_delayed);

% Convert time from seconds to minutes
t_minutes = t / 60;

% Plot the step response with the time in minutes
plot(t_minutes, y);
title('Step Response of the Temperature Control System with Delay');
xlabel('Time (minutes)');
ylabel('Temperature (\Delta T)');

% Bode plot
%figure;
%bode(G_delayed);
%title('Bode Plot of the Transfer Function with Delay');

% Pole-Zero Map
%figure;
%pzmap(G_delayed);
%title('Pole-Zero Map of the Transfer Function with Delay');

% Gain and Phase Margins
%figure;
%margin(G_delayed);
%title('Gain and Phase Margins of the System with Delay');

% Nyquist Plot
%figure;
%nyquist(G_delayed);
%title('Nyquist Plot of the Transfer Function with Delay');

% Design a PID Controller
Kp = 1000; Ki = 1; Kd = 0; % Adjust these values based on the design requirements
C = pid(Kp, Ki, Kd);
sys_cl = feedback(C*G_delayed, 1); % Closed-loop system with PID controller

% Step response of the closed-loop system
figure;
step(sys_cl);
title('Step Response of the Closed-Loop System with PID Controller and Delay');
xlabel('Time (s)');
ylabel('Temperature (\Delta T)');

% Bode Plot of the closed-loop system
%figure;
%bode(sys_cl);
%title('Bode Plot of the Closed-Loop System with PID Controller and Delay');

% Display the transfer function of the closed-loop system with delay
disp('The transfer function of the closed-loop system with delay is:')
disp(sys_cl)