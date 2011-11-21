function SlnPlotTwoClasses(sln, X, T)
%SlnPlotTwoClasses Plot descrimination line for a two-class problem
%with two input features and associated data
%
%Description
%SlnPlotTwoClasses(sln, X, T) plots the descrimination line 
%for a two-class problem with two input features which was learned by a 
%single-layer network sln. Associated (training) data with X being the 
%feature matrix of the data (columns being the (training) instances) and 
%T being the true class vector (a row vector).
    x = min(X(1,:)):0.2:max(X(1,:));
    y = min(X(2,:)):0.2:max(X(2,:));
    [xm, ym] = meshgrid(x,y);
    xm = xm(:);
    ym = ym(:);
    grid = [xm ym]';
    Z = SlnApplyMany(sln, grid);
    Z = reshape(Z, length(y), length(x));
    v = [ 0 0 ]; %[-0.5 0 0.5]; %[0.1 0.5 0.9];
    [c, h] = contour(x, y, Z, v);
    title('Single-Layer Network for two classes')
    set(h, 'linewidth', 3)
    clabel(c, h);

    hold on;
    plot(X(1, T<=0), X(2, T<=0),'bo');
    plot(X(1, T==1), X(2, T==1),'rx');
    hold off;
    
    drawnow;
end