db = [-20 : 5 : 20];
% To get below SNR result, please see readme.txt
dbMale = [...
0.34 2.05 5.22 8.28 11.68 15.01 18.26 20.77 24.05; % AWGN
6.79 8.95 9.64 11.30 12.14 15.42 18.68 22.38 26.30; % 'factory1'
11.64 12.70 13.11 14.1491 15.75 17.92 21.09 25.11 29.13; % 'factory2'
18.87 19.89 22.28 24.19 25.31 27.35 28.43 29.22 30.64; % volvo
7.46 8.68 9.70 10.61 12.83 16.31 18.16 20.94 24.72]; % 'Brickcom_CB-102Ap_G711.wav'

dbFemale = [...
0.72 2.78 6.05 9.78 13.10 16.34 19.64 22.26 25.72;
7.59 8.40 10.45 11.95 12.29 15.65 19.11 23.34 27.50;
11.29 12.16 13.20 14.99 15.68 18.41 21.76 25.77 29.44;
17.70 18.76 22.17 24.82 25.68 27.64 29.05 30.48 32.70;
7.64 8.62 10.51 11.25 12.53 16.14 18.44 21.24 25.47];

figure
marker = ['o'; 's'; '^'; 'd'; '<'];
colour = ['r'; 'b'; 'k'; 'g'; 'c'];
titleStr = {'Male'; 'Female'};
dbSNR = [];
dbSNR{1} = dbMale;
dbSNR{2} = dbFemale;
noiseName = {'AWGN';
	'Factory1 Noise';
	'Factory2 Noise';
	'Volvo Noise';
	'Server Room Noise';};

for jj = 1 : 2
	subplot(1, 2, jj)
	hold on
	for ii = 1 : size(dbSNR{jj}, 1)
		plot(db, dbSNR{jj}(ii, :), 'Marker', marker(ii), ...
			'Color', colour(ii), ...
			'MarkerFaceColor', colour(ii), ...
			'DisplayName', noiseName{ii})
		for kk = 1 : size(dbSNR{jj}, 2)
			text(db(kk), dbSNR{jj}(ii, kk), ...
				num2str(dbSNR{jj}(ii, kk)), ...
				'HorizontalAlignment','left', ...
				'VerticalAlignment','bottom');
		end
	end
	ylabel('SNR_{filtered} (dB)')
	xlabel('SNR_{noisy} (dB)')
	set(gca, 'XTick', db)
	xlim([db(1) db(end)])
	ylim([0 35])
	title(['SNR for ' titleStr{jj} ' Voice'])
	legend('Show', 'Location', 'southeast')
end
