while 1
    f = fopen('measurement.txt', 'r');
    m = fscanf(f,'%f')
    fclose(f);
end
