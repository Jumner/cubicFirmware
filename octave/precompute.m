pkg load control
close all

function u = getU(K, x)
	u = - K * x;
end
% Values
s = -0.001; % Small neg value
mass = 1.199;	      % Mass in kg
l = 0.119511505722;			 % Length of pendulum arm
ix = 0.004281;			 % Inertia x
iy = 0.02183;			 % Inertia y
iz = 0.02189;			 % Inertia z
iw = 0.0002504;		 % Wheen inertia

% A
y = mass * 9.81 * l / iy; % Y Gravity
z = mass * 9.81 * l / iz; % Z Gravity
A = [0 0 0 1 0 0 0 0 0;
			0 0 0 0 1 0 0 0 0;
			0 0 0 0 0 1 0 0 0;
			0 0 0 0 0 0 0 0 0;
			0 y 0 0 0 0 0 0 0;
			0 0 z 0 0 0 0 0 0;
			0 0 0 0 0 0 s 0 0;
			0 0 0 0 0 0 0 s 0;
			0 0 0 0 0 0 0 0 s];

% B
w = 1 / iw;							 % Wheel acceleration
x = -1 / (sqrt(3) * ix); % X Acceleration
y = -2 / (sqrt(6) * iy); % Y Acceleration
h = 1 / (sqrt(6) * iy);	 % Half Y Acceleration
z = 1 / (sqrt(2) * iz); % Z Acceleration
n = -1 / (sqrt(2) * iz);	 % Negative Z Acceleration (its acc positive)
B = [0 0 0;
		 0 0 0;
		 0 0 0;
		 x x x;
		 y h h;
		 0 z n;
		 w 0 0;
		 0 w 0;
		 0 0 w];

% C
C = eye(9);

% D
D = zeros(9,3);

% Q
Q = [0.0000002 0 0 0 0 0 0 0 0;
		 0 0.005 0 0 0 0 0 0 0;
		 0 0 0.005 0 0 0 0 0 0;
		 0 0 0 0.0000005 0 0 0 0 0;
		 0 0 0 0 0.1 0 0 0 0;
		 0 0 0 0 0 0.1 0 0 0;
		 0 0 0 0 0 0 1 0 0;
		 0 0 0 0 0 0 0 1 0;
		 0 0 0 0 0 0 0 0 1];

% R
R = [10000000000 0 0;
		 0 10000000000 0;
		 0 0 10000000000];

K = lqr(A,B,Q,R);
disp(K);

sys = ss((A - B*K), B, C ,D);
x0 = [-1;
			0.02;
			0.02;
			0;
			0;
			0;
			0;
			0;
			0];
t= 0:0.005:1.5;
[y,t,x] = initial(sys, x0, t);
% u = -x*K

% Graph

for i = 1:length(t)
	u(i,:) = getU(K, x(i,:)');
end

graphLQR = helper('GetGraph'); % Grab graph function
graphLQR(x, t, u); % Graph data