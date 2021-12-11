function h = helper(option) % Called to grab functions
	if strcmp(option, 'GetGraph') % If its the graph function
		h = @graphLQR; % Return it
		return
	elseif strcmp(option, 'GetsimulatePendulum')
		h = @simulatePendulum; % Return it
		return
	end
	h = 0 % Bad call
	return
end

function graphLQR(x, t, u) % Graphs lqr data
	uLen = length(u(1, :)); % Length of u vector
	xLen = length(x(1, :)); % Length of state vector
	share = 1/(xLen + uLen); % Decimal that each graph is allowed
	timeTaken = t(length(t));
	
	fig = figure('Position', [800 0 1400 800], 'Name', 'Plot figure'); % Create window with title of time taken
	%pause(1); % Wait for data so graphs insta display
	for j = 1:xLen+uLen % For each graph
		if j <= xLen % If state
			PlotArray{j*3} = subplot(xLen,2,j*2-1);
			PlotArray{j*3-2} = area(t, x(:, j)); % Area under graph
			hold on % Keep it so they overlap
			PlotArray{j*3-1} = plot(t, x(:, j), 'r', "linewidth", 2); % Line
			title(['x', num2str(j)]); % Title it right
		else % If input
			PlotArray{j*3} = subplot(uLen,2,(j-xLen)*2);
			PlotArray{j*3-2} = area(t, u(:, j-xLen)); % Area under graph
			hold on % Keep it so they overlap
			PlotArray{j*3-1} = plot(t, u(:, j-xLen), 'r', "linewidth", 2); % Line
			title(['u', num2str(j-xLen)]); % Title it right
		end
		% PlotArray{j*3-1}.LineWidth = 2; % Set the line width
		xlabel('Time'); % Label time axis
		xlim([0 timeTaken]) % Limit it so its more usefull
	end
	waitfor(fig);
end