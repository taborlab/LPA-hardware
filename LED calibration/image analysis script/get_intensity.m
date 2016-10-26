function intensity = get_intensity(filename, threshold)
    im = imread(filename);
    
    % Grab the R,G, or B component of the photo
    im = squeeze(im(:,:,1));
    
    %im = double(im);
    figure, imshow(im);
    
    %% Background counts by samping image border
    
    % Generate a border mask
    border_mask = zeros(size(im));
    border_mask(1:25,:) = 1;
    border_mask(end-25:end,:) = 1;
    border_mask(:,1:25) = 1;
    border_mask(:,end-25:end) = 1;
    n = nnz(border_mask); % number of nonzero elements
    
    % Mask image and calculate avg background counts
    border_mask_im = im;
    border_mask_im(~border_mask)=0;
    figure, imshow(border_mask_im);
    bgnd_avg = 1/n*sum(sum(border_mask_im))
    
    %% Subtract background from image
    im = im-bgnd_avg;
    figure, imshow(im)
    
    %% Thresholding
    % First pass identifies regions corresponding to each well
    if length(nargin) == 1
        threshold = 10000;
    end
    im_b = (im >= threshold);
    %figure, imshow(im_b)
    
    % Get regions based on thresholding
    cc = bwconncomp(im_b, 4)
%     disp([num2str(cc.NumObjects) ' objects found.'])
%     well_data = regionprops(cc, 'basic');
    
    % Only keep the 24 largest regions identified
    numPixels = cellfun(@numel,cc.PixelIdxList);
    numPixels_sorted = sort(numPixels);
    area_threshold = numPixels_sorted(end-24+1);
    
    filtered_regions = zeros(size(im));
    for i = 1:numel(cc.PixelIdxList)
        region_size = numel(cc.PixelIdxList{i});
        if (region_size >= area_threshold)
            filtered_regions(cc.PixelIdxList{i}) = 1;
        end
    end
    
    figure, imshow(filtered_regions)
    
    % Second pass makes a new data structure for the remaining regions
    % for easy access to the region parameters (e.g. centroid)
    cc = bwconncomp(filtered_regions, 4);
    disp([num2str(cc.NumObjects) ' objects found.'])
    well_data = regionprops(cc, 'basic');
    
    well_center = reshape([well_data.Centroid], [2,24]);
    
    % Sort wells
    well_center = sortrows(well_center',1);
    for i = 1:6
        well_center(4*(i-1) + 1:4*i,:) = sortrows(well_center(4*(i-1) + 1:4*i,:),2);
    end
    well_center = well_center';
    % Make regions based on circles with fixed radius
    masks = {};
    all_masks = zeros(size(im));
    
    radius = 60;  % pixels: 30 is good for imager, 150 for eocam, 42 for eocamsmall
    
    for i = 1:24
        center = well_center(:,i);
        [W,H] = meshgrid(1:size(im, 1),1:size(im, 2));
        mask{i} = (sqrt((W-center(2)).^2 + (H-center(1)).^2) < radius)';
        all_masks = all_masks + mask{i};
    end
    figure, imshow(all_masks)
    hold on
    for i = 1:24
        text(well_center(1,i), well_center(2,i), num2str(i), 'HorizontalAlignment','center')
    end
    % Get total well intensities
    intensity = zeros(4, 6);
    for i = 1:24
        intensity(rem(i-1,4) + 1, floor((i-1)/4) + 1) = sum(im(mask{i}));
    end
    
end