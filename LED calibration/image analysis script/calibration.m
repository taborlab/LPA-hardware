clear; close all; clc
%% Constants
max_cal = 255;
rows = 4;
columns = 6;
channels_per_well = 2;

tot_cols = columns*channels_per_well;
%% Load intensities and previous calibration

channel = 2; %1=R, 2=G
calibration_round = 1;

folder_ch1 = 'example/arya_678nm_bot_150802/';
folder_ch2 = 'example/arya_678nm_bot_150802/';
folders = char(folder_ch1, folder_ch2);
filename = [strtrim(folders(channel,:)) 'round1_raw.csv'];

%% Calculate calibration values
cals = zeros(rows, columns);
intensities = csvread(filename);
if calibration_round >= 2
    cal_prev = csvread([strtrim(folders(channel,:)) 'calibration_round_ch' num2str(channel) '_n' num2str(calibration_round - 1) '.csv']);
    intensities = intensities ./ (cal_prev/max_cal);
end
min_int = min(min(intensities));
rel_intensity = intensities / min_int;
cal = 1./rel_intensity;
cal = round(cal * max_cal);
% Save calibration values
dlmwrite([strtrim(folders(channel,:)) 'calibration_round_ch' num2str(channel) '_n' num2str(calibration_round) '.csv'],cal, 'delimiter', ',', 'precision', 9);

% Plot results
rel_intensities = intensities/max(max(intensities));
rel_cal_intensities = intensities.*cal/max(max(intensities.*cal));
figure,
pos = get(gcf, 'Position');
pos(2) = 100;
pos(4) = pos(4)*2;
set(gcf, 'Position', pos);
sp = 310;
data = {rel_intensities, cal, rel_cal_intensities};
titles = {'Intensities', 'Calibration values', 'Expected result'};
for i = 1:3
    subplot(sp + i)
    imagesc(data{i}); 
    colorbar;
    title(titles{i});
end

%% Combine with other channels
% Calibration round to load per channel
calibration_round_channels = [1 1];

calibration_round_channels(channel) = calibration_round;
cals = zeros(rows, tot_cols);
for i = 1:channels_per_well
    calibration_round = calibration_round_channels(i);
    if calibration_round >= 1
        cal = csvread([strtrim(folders(i,:)) 'calibration_round_ch' num2str(i) '_n' num2str(calibration_round) '.csv']);
        cals(:, i:channels_per_well:tot_cols) = cal;
    else
        cals(:, i:channels_per_well:tot_cols) = max_cal;
    end
end

%% Print
for j = 1:rows
    si = [];
    for i = 1:tot_cols
        si = [si num2str(cals(j,i)) ', '];
    end
    disp(si)
end
