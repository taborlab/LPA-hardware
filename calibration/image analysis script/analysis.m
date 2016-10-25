clear; close all; clc
%% Load image
folder = 'example/arya_678nm_bot_150802/'
im_filename = [folder 'round1'];
intensity_normal = get_intensity([im_filename '.tif']);
intensity_transp = get_intensity([im_filename '_T.tif']);

%% Processing
% Transpose data for transposed image
intensity_transp = reshape(intensity_transp(end:-1:1), size(intensity_transp,1), size(intensity_transp,2));
% Average to get true intensity
intensity = (intensity_normal + intensity_transp)/2;

% Plot data
rel_normal = intensity_normal/max(max(intensity_normal));
rel_transp = intensity_transp/max(max(intensity_transp));
rel_avg = intensity/max(max(intensity));

min_int = min([min(min(rel_normal)) min(min(rel_transp))]);
plot(rel_normal, rel_transp, 'o', min_int:0.01:1,min_int:0.01:1,'--')
xlabel('Normal position')
ylabel('Rotated position')
grid on

figure,
pos = get(gcf, 'Position');
pos(2) = 100;
pos(4) = pos(4)*2;
set(gcf, 'Position', pos);
sp = 310;
data = {rel_normal, rel_transp, rel_avg};
titles = {'Normal', 'Transposed', 'Average'};
for i = 1:3
    subplot(sp + i)
    imagesc(data{i}); 
    colorbar;
    title(titles{i});
end

%% Save
mean_intensity = mean(mean(intensity));
s = std(intensity(:));
disp(['Standard deviation: ' num2str(s/mean_intensity*100) '%'])
int_rel = (intensity - mean_intensity)/mean_intensity*100;
dlmwrite([im_filename '_raw.csv'],intensity, 'delimiter', ',', 'precision', 9);
dlmwrite([im_filename '_rel.csv'],int_rel, 'delimiter', ',', 'precision', 9);