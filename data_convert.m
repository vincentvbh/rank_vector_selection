function [Avg, Min, Max, IndexV] = data_convert(file_name)
    fileID = fopen(file_name, 'r');
    tmp_data = fscanf(fileID, '%d');
    chunk_number = length(tmp_data) / 13;
    Avg = []; Min = []; Max = []; IndexV = [];
    for i = 1:chunk_number
        tmp_size = tmp_data((i - 1) * 13 + 1);
        IndexV = [IndexV; tmp_size];
        tmp_avg = []; tmp_min = []; tmp_max = [];
        for j = 2:5
            tmp_avg = [tmp_avg; tmp_data((i - 1) * 13 + j)];
        end
        for j = 6:9
            tmp_min = [tmp_min;tmp_data((i - 1) * 13 + j)];
        end
        for j = 10:13
            tmp_max = [tmp_max; tmp_data((i - 1) * 13 + j)];
        end
        Avg = [Avg, tmp_avg];
        Min = [Min, tmp_min];
        Max = [Max, tmp_max];
    end
    fclose(fileID);
end