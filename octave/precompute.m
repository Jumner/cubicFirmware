pkg load control
close all

function u = getU(K, x)
	u = - K * x;
end
% Values
mass = 1.199;	      % Mass in kg
l = 0.119511505722;			 % Length of pendulum arm
ix = 0.016;			 % Inertia x
iy = 0.016;			 % Inertia y
iz = 0.016;			 % Inertia z
iw = 0.00025; % Wheel inertia


% A
x = mass * 9.81 * l / (ix); % X Gravity
y = mass * 9.81 * l / (iy); % Y Gravity
z = mass * 9.81 * l / (iz); % Z Gravity
A = [0 0 0 1 0 0 0 0 0;
			0 0 0 0 1 0 0 0 0;
			0 0 0 0 0 1 0 0 0;
			x 0 0 0 0 0 0 0 0;
			0 y 0 0 0 0 0 0 0;
			0 0 z 0 0 0 0 0 0;
			0 0 0 0 0 0 0 0 0;
			0 0 0 0 0 0 0 0 0;
			0 0 0 0 0 0 0 0 0];

% B
w = 1 / iw;							 % Wheel acceleration
x = -1 / ix;
y = -1 / iy;
z = -1 / iz;
B = [0 0 0;
		 0 0 0;
		 0 0 0;
		 x 0 0;
		 0 y 0;
		 0 0 z;
		 w 0 0;
		 0 w 0;
		 0 0 w];

% C
C = eye(9);

% D
D = zeros(9,3);

% Q
Q = [1 0 0 0 0 0 0 0 0;
		 0 1 0 0 0 0 0 0 0;
		 0 0 1 0 0 0 0 0 0;
		 0 0 0 1 0 0 0 0 0;
		 0 0 0 0 1 0 0 0 0;
		 0 0 0 0 0 1 0 0 0;
		 0 0 0 0 0 0 100 0 0;
		 0 0 0 0 0 0 0 100 0;
		 0 0 0 0 0 0 0 0 100];

% R
R = [10000000 0 0;
		 0 10000000 0;
		 0 0 10000000];

K = lqr(A,B,Q,R);
disp(K);

sys = ss((A - B*K), B, C ,D);
x0 = [0.05;
			0;
			0;
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