close all
% Values
s = -0.00001; % Small neg value
mass = 0.8;	 % Mass in kg
l = 0.4;			 % Length of pendulum arm
ix = 1;			 % Inertia x
iy = 1;			 % Inertia y
iz = 1;			 % Inertia z
iw = 0.5;		 % Wheen inertia

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
z = -1 / (sqrt(2) * iz); % Z Acceleration
n = 1 / (sqrt(2) * iz);	 % Negative Z Acceleration (its acc positive)
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
Q = [1 0 0 0 0 0 0 0 0;
		 0 1 0 0 0 0 0 0 0;
		 0 0 1 0 0 0 0 0 0;
		 0 0 0 1 0 0 0 0 0;
		 0 0 0 0 1 0 0 0 0;
		 0 0 0 0 0 1 0 0 0;
		 0 0 0 0 0 0 1 0 0;
		 0 0 0 0 0 0 0 1 0;
		 0 0 0 0 0 0 0 0 1];

% R
R = [1 0 0;
		 0 1 0;
		 0 0 1];

K = lqr(A,B,Q,R);
disp(K);