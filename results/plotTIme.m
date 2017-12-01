filename = 'hedcuter.txt';
formatSpec = '%5s%6s%f%[^\n\r]';
fileID = fopen(filename,'r');
dataArray = textscan(fileID, formatSpec, 'Delimiter', '', 'WhiteSpace', '', 'TextType', 'string',  'ReturnOnError', false);
dataArray{1} = strtrim(dataArray{1});
dataArray{2} = strtrim(dataArray{2});
fclose(fileID);
hedcuter = table(dataArray{1:end-1}, 'VariableNames', {'Total','time','VarName3'});
clearvars filename formatSpec fileID dataArray ans;

filename = 'voronoi.txt';
formatSpec = '%9s%3s%5f%s%[^\n\r]';
fileID = fopen(filename,'r');
dataArray = textscan(fileID, formatSpec, 'Delimiter', '', 'WhiteSpace', '', 'TextType', 'string',  'ReturnOnError', false);
dataArray{1} = strtrim(dataArray{1});
dataArray{2} = strtrim(dataArray{2});
dataArray{4} = strtrim(dataArray{4});
fclose(fileID);
voronoi = table(dataArray{1:end-1}, 'VariableNames', {'Completed','in','VarName3','seconds'});
clearvars filename formatSpec fileID dataArray ans;

hedcuter = table2array(hedcuter(:,3));
voronoi = table2array(voronoi(:,3));




h=figure;
plot(200:200:2000,hedcuter,'-^r','DisplayName','hedcuter');
hold on
plot(200:200:2000,voronoi,'-ob','DisplayName','voronoid');
set(gca,'FontSize',20)
xlabel('Number of the disks')
ylabel('Time(s)')
%set(gca,'YLim',[0 40])
legend('show')
%set(gca, 'XTickLabelRotation',45);
%xticks([1 2 3 4])
%xticklabels({'Bunny-48','Bunny-64','Bunny-96','Bunny-128'})
ouputfile = strcat('../report/figs/','time2');
saveas(h,ouputfile,'epsc');