function plot_one(data, IndexV, title_name, log_scale)
    [m, n] = size(data);
    marker_list = '+ox.';
    line_color = 'rgbm';
    for i = 1:m
       plot(IndexV, data(i, :), ['-', marker_list(i), line_color(i)]);
       hold on;
    end
    if log_scale == 1
       xlabel('log(n)'); 
    end
    title(title_name);
    legend('STL sort', 'Purly-partition', 'Nearest-median-of-A', 'Median-of-Q', 'Location', 'northwest');
    legend('boxoff');
    hold off;
end