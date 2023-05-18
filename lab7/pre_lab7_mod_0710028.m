%% BPSK transmission over AWGN channel
close all;clear all;clc;           % BPSK
dist=100:100:400;        % distance in meters
PtdBm=10;                % transmit power in dBm
PndBm=-85;              % noise power in dBm
Pt=10^(PtdBm/10)/1000;  % transmit power in watt
Pn=10^(PndBm/10)/1000;  % noise power in watt
Bit_Length=1e3;         % number of bits transmitted

%% Friss Path Loss Model
Gt=1;
Gr=1;
freq=2.4e9;
c_speed=3e8;

% TODO: Calculate Pr(d)
%Pr=ones(length(dist),1);    % TODO: replace this with Friis' model
for d=1:length(dist)
    Pr(d)=Pt*Gt*Gr*((c_speed/(4*pi*dist(d)*freq))^2);
end
%% BPSK Transmission over AWGN channel
tx_data = randi(2, 1, Bit_Length) - 1;                  % random between 0 and 1
%% TODO-2
%% BPSK: {1,0} -> {1+0i, -1+0i}
%% QPSK: {11,10,01,00} -> {1+i, -1+i, -1-i, 1-i} * scaling factor
%% 16QAM: {1111, 1110, 1101, 1100, 1011, 1010, 1001, 1000, 0111, 0110, 0101, 0100, 0011, 0010, 0001, 0000}
%% -> {3a+3ai, 3a+ai, a+3ai, a+ai, -a+3ai, -3a+3ai, -3a+ai, -a+ai, 3a-ai, 3a-3ai, a-ai, a-3i, -a-ai, -a-3ai, -3a-ai, -3a-3ai}
n=(randn(1,Bit_Length)+randn(1,Bit_Length)*i)/sqrt(2);  % AWGN noises
n=n*sqrt(Pn);
QAM=[3+3i 3+1i 1+3i 1+1i -1+3i -3+3i -3+1i -1+1i 3-1i 3-3i 1-1i 1-3i -1-1i -1-3i -3-1i -3-3i]./sqrt(10);
QPSK=[1+1i -1+1i -1-1i 1-1i]./sqrt(2);    	
for mod_order=[1,2,4]
	if mod_order==1
    	x(mod_order,:)=(tx_data.*2-1)+0i;                                    % TODO-2: change it to three different modulated symbols
    elseif mod_order==2
    	for k=1:(length(tx_data)/2)
    		tx=tx_data(2*k-1)*2+tx_data(2*k);
    		x(mod_order,k)=QPSK(4-tx);
    	end
    elseif mod_order==4
    	for k=1:(length(tx_data)/4)
    		tx=tx_data(4*k)+tx_data(4*k-1)*2+tx_data(4*k-2)*4+tx_data(4*k-3)*8; 
    		x(mod_order,k)=QAM(16-tx);
    	end
    end
    for d=1:length(dist)
        y(mod_order,d,:)=sqrt(Pr(d))*x(mod_order,:)+n;
    end
end

%% Equalization
% Detection Scheme:(Soft Detection)
% +1 if o/p >=0
% -1 if o/p<0
% Error if input and output are of different signs


for mod_order=[1,2,4]
    figure('units','normalized','outerposition',[0 0 1 1])
	sgtitle(sprintf('Modulation order: %d', mod_order)); 
    for d=1:length(dist)
        % TODO: s = y/Pr
        % TODO: x_est = 1 if real(s) >= 0; otherwise, x_est = -1
        s(:)=y(mod_order,d,:)/sqrt(Pr(d));
        wrong_bits=0;
        if mod_order==1
        	for cnt=1:1000
        		if real(s(cnt))>=0
        			x_est=1;
        		else
        			x_est=-1;
        		end
        		if x_est~=x(mod_order,cnt)
        			wrong_bits=wrong_bits+1;
        		end	
        	end

        elseif  mod_order==2
        	for cnt=1:500
        		if real(s(cnt))>=0
        			x_est=1/sqrt(2);
        		else
        			x_est=-1/sqrt(2);
        		end
        		if imag(s(cnt))>=0
        			x_est=x_est+(1i/sqrt(2));
        		else
        			x_est=x_est-(1i/sqrt(2));
        		end
        		if x_est~=x(mod_order,cnt)
	        		for j=1:length(QPSK)
	        			if x_est==QPSK(j)
	        				x_est=4-j;
	        				break;
	        			end
	        		end
	        		x_demod=0;
	        		for j=1:length(QPSK)
	        			if x(mod_order,cnt)==QPSK(j)
	        				x_demod=4-j;
	        				break;
	        			end
	        		end
	        		if mod(real(x_est),2)~=mod(real(x_demod),2)
	        			wrong_bits=wrong_bits+1;
	        			x_est=x_est/2;
	        			x_demod=x_demod/2;
	        		end
	        		if mod(real(x_est),2)~=mod(real(x_demod),2)
	        			wrong_bits=wrong_bits+1;
	        		end
        		end	
        	end 
        elseif mod_order==4
        	for cnt=1:250
        		if real(s(cnt))>=2/sqrt(10)
        			x_est=3/sqrt(10);
        		elseif real(s(cnt))>=0/sqrt(10)
        			x_est=1/sqrt(10);
        		elseif real(s(cnt))>=-2/sqrt(10)
        			x_est=-1/sqrt(10);
        		else
        			x_est=-3/sqrt(10);
        		end
        		if imag(s(cnt))>=2/sqrt(10)
        			x_est=x_est+(3i/sqrt(10));
        		elseif imag(s(cnt))>=0/sqrt(10)
        			x_est=x_est+(1i/sqrt(10));
        		elseif imag(s(cnt))>=-2/sqrt(10)
        			x_est=x_est-(1i/sqrt(10));
        		else
        			x_est=x_est-(3i/sqrt(10));
        		end
        		if x_est~=x(mod_order,cnt)
	        		for j=1:length(QAM)
	        			if x_est==QAM(j)
	        				x_est=16-j;
	        				break;
	        			end
	        		end
	        		x_demod=0;
	        		for j=1:length(QAM)
	        			if x(mod_order,cnt)==QAM(j)
	        				x_demod=16-j;
	        				break;
	        			end
	        		end
	        		if mod(real(x_est),2)~=mod(real(x_demod),2)
	        			wrong_bits=wrong_bits+1;
	        			x_est=x_est/2;
	        			x_demod=x_demod/2;
	        		end
	        		if mod(real(x_est),2)~=mod(real(x_demod),2)
	        			wrong_bits=wrong_bits+1;
	        			x_est=x_est/2;
	        			x_demod=x_demod/2;
	        		end
	        		if mod(real(x_est),2)~=mod(real(x_demod),2)
	        			wrong_bits=wrong_bits+1;
	        			x_est=x_est/2;
	        			x_demod=x_demod/2;
	        		end
	        		if mod(real(x_est),2)~=mod(real(x_demod),2)
	        			wrong_bits=wrong_bits+1;
	        		end
        		end	
        	end 
        end

        noise=x(mod_order,:)-s;

        Ex=1000/mod_order;
        
        En=0;
        for cnt=1:(1000/mod_order)
        	En=En+(real(noise(cnt)))^2+(imag(noise(cnt)))^2;
        end

        SNR(d,mod_order)=Pr(d)/Pn;
        SNRdB(d,mod_order)=10*log10(SNR(d,mod_order));
        BER_simulated(d,mod_order)=wrong_bits/Bit_Length;
        SNRdB_simulated(d,mod_order)=10*log10(Ex/En);
        % TODO-2: demodulate x_est to x' for various modulation schemes and calculate BER_simulated(d)
        % TODO: noise = s - x, and, then, calculate SNR_simulated(d)
        subplot(2, 2, d)
        hold on;
        plot(s(1:(1000/mod_order)),'bx');       % TODO: replace y with s
        plot(real(x(mod_order,1:(1000/mod_order))),imag(x(mod_order,1:(1000/mod_order))),'ro');
        hold off;
        xlim([-2,2]);
        ylim([-2,2]);
        title(sprintf('Constellation points d=%d', dist(d)));
        legend('decoded samples', 'transmitted samples');
        grid
    end
    filename = sprintf('IQ_%d.jpg', mod_order);
    saveas(gcf,filename,'jpg')
end

%% TODO-2: modify the figures to compare three modulation schemes
figure('units','normalized','outerposition',[0 0 1 1])
hold on;
semilogy(dist,SNRdB_simulated(:,1),'bo-','linewidth',2.0);
semilogy(dist,SNRdB_simulated(:,2),'rv--','linewidth',2.0);
semilogy(dist,SNRdB_simulated(:,4),'mx-.','linewidth',2.0);
hold off;
title('SNR');
xlabel('Distance [m]');
ylabel('SNR [dB]');
legend('BPSK','QPSK','16QAM');
axis tight 
grid
saveas(gcf,'SNR.jpg','jpg')

figure('units','normalized','outerposition',[0 0 1 1])
hold on;
semilogy(dist,BER_simulated(:,1),'bo-','linewidth',2.0);
semilogy(dist,BER_simulated(:,2),'rv--','linewidth',2.0);
semilogy(dist,BER_simulated(:,4),'mx-.','linewidth',2.0);
hold off;
title('BER');
xlabel('Distance [m]');
ylabel('BER');
legend('BPSK','QPSK','16QAM');
axis tight 
grid
saveas(gcf,'BER.jpg','jpg')
return;
