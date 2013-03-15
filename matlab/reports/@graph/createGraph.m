function o = createGraph(o)
%function o = createGraph(o)
% Create the graph
%
% INPUTS
%   o   [graph] graph object
%
% OUTPUTS
%   o   [graph] graph object
%
% SPECIAL REQUIREMENTS
%   none

% Copyright (C) 2013 Dynare Team
%
% This file is part of Dynare.
%
% Dynare is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% Dynare is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with Dynare.  If not, see <http://www.gnu.org/licenses/>.

assert(~isempty(o.data));

if ~isempty(o.figname)
    warning('@graph.createGraph: will overwrite %s with new graph\n', ...
            o.figname);
end

%o = readConfig(o);

disp('creating plot..........');
h = figure('visible','off');
hold on;
box on;
if o.grid
    grid on;
    set(gca, 'GridLineStyle', '--');
end
%set(0, 'CurrentFigure',h);
%set(h, 'PaperPositionMode', 'auto');
%set(h, 'units', 'normalized', 'outerposition', [0 0 1 1]);

if isempty(o.seriestouse)
    ds = o.data;
else
    ds = o.data{o.seriestouse{:}};
end

if ~isempty(o.xrange)
    dd1 = dynDate(o.xrange{1});
    dd2 = dynDate(o.xrange{2});
    ds  = ds(dd1:dd2);
end
data = ds.data;

x = 1:1:ds.nobs;
xlabels = getDatesCellStringArray(ds.time);

plot(x, data);

if ~isempty(o.yrange)
    ylim([o.yrange{:}]);
end

if ~isempty(o.shade)
    x1 = find(strcmp(o.shade{1}, xlabels));
    x2 = find(strcmp(o.shade{2}, xlabels));
    assert(~isempty(x1) && ~isempty(x2), ['@graph.createGraph: either ' ...
                        o.shade{1} ' or ' o.shade{2} 'is not in the date ' ...
                        'range of data selected.']);
    yrange = get(gca, 'YLim');

    % From ShadePlotForEmpahsis (Matlab Exchange)
    % use patch bc area doesn't work with matlab2tikz
    patch([repmat(x1, 1, 2) repmat(x2, 1, 2)], [yrange fliplr(yrange)], ...
          'b', 'FaceAlpha', .2);
end

set(gca,'XTick', x);
set(gca,'XTickLabel', xlabels);

if o.legend
    lh = legend(ds.name);
    set(lh, 'orientation', o.legend_orientation);
    set(lh, 'Location', o.legend_location);
    set(lh, 'FontSize', o.legend_font_size);
    legend('boxoff');
end

if ~isempty(o.xlabel)
    xlabel(['$\textbf{\footnotesize ' o.xlabel '}$'], 'Interpreter', 'LaTex');
end

if ~isempty(o.ylabel)
    ylabel(['$\textbf{\footnotesize ' o.ylabel '}$'], 'Interpreter', 'LaTex');
end

if ~isempty(o.title)
    title( o.title, 'Interpreter', 'LaTex');
end
drawnow;

o.figname = [tempname '.tex'];
disp('  converting to tex....');
matlab2tikz('filename', o.figname, ...
            'showInfo', false, ...
            'showWarnings', false, ...
            'checkForUpdates', false);

grid off;
box off;
hold off;
close(h);
clear h;
end
