function SlnPlotThreeClasses(sln, X, T)
%SlnPlotTwoClasses Plot descrimination line for a three-class problem
%with two input features and associated data
%
%Description
%SlnPlotThreeClasses(sln, X, T) plots the descrimination regions 
%for a three-class problem with two input features which was learned by a 
%single-layer network sln. Associated (training) data with X being the 
%feature matrix of the data (columns being the (training) instances) and 
%T being the true class matrix (matrix with three row vectors, one for
%the output of each output neuron).
    x = min(X(1,:)):0.2:max(X(1,:));
    y = min(X(2,:)):0.2:max(X(2,:));
    [xm, ym] = meshgrid(x,y);
    xm = xm(:);
    ym = ym(:);
    grid = [xm ym]';
    Z = SlnApplyMany(sln, grid);
    class= sln.postProcessFn(Z);

    colors = ['b.'; 'r.'; 'g.'];
    for i = 1:3
      thisX = xm(class == i);
      thisY = ym(class == i);
      h = plot(thisX, thisY, colors(i,:));
      hold on;
      set(h, 'MarkerSize', 8);
    end

    T1=sln.postProcessFn(T);
    plot(X(1, T1<=1), X(2, T1<=1),'bo');
    plot(X(1, T1==2), X(2, T1==2),'rx');
    plot(X(1, T1==3), X(2, T1==3),'g*');

    hold off;
    title('Plot of Decision regions')
    
    drawnow;
end