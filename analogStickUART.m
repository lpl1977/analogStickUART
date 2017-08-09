classdef analogStickUART < handle
    %analogStickUART serial communication with analog stick
    %  Based on ATmega328p microcontroller
    %
    %  Lee Lovejoy
    %  August 9, 2017
    %  ll2833@columbia.edu
    
    properties (SetAccess=private)
        serialObj
        portName = '/dev/ttyUSB0';
        baud = 115200;
        inputBufferSize = 2048;        
        nBytesPerSample = 8;
    end
    
    methods
        
        %  Class constructor
        function obj = analogStickUART(varargin)
            
            %  Update parameter values
            for i=1:2:nargin
                if(any(strcmp(properties(mfilename),varargin{i})))
                    obj.(varargin{i}) = varargin{i+1};
                end
            end
            
            %  Create serial port object
            obj.serialObj = serial(obj.portName,'baud',obj.baud);
            obj.serialObj.InputBufferSize = obj.inputBufferSize;
            fopen(obj.serialObj);
        end        
        
        %  Get precise time
        %
        %  Write two bytes to serial and read resulting line
        function t = ticksSinceStart(obj)
            fwrite(obj.serialObj,uint8([0 0]));
            t = fread(obj.serialObj,1,'uint32');
            fread(obj.serialObj,2,'int16');
        end
        
        %  Toggle sample write
        function obj = toggleSampleWriting(obj)
            fwrite(obj.serialObj,uint8(0));            
        end
        
        %  Read samples
        function [timeStamps,samples,msg] = readSamples(obj)
            nBytes = obj.serialObj.BytesAvailable;
            nSamples = floor(nBytes / obj.nBytesPerSample);
            data = fread(obj.serialObj,[obj.nBytesPerSample nSamples],'uchar');
            timeStamps = data(1:4,:);
            timeStamps = typecast(uint8(timeStamps(:)),'uint32');
            samples = data(5:end,:);
            samples = reshape(typecast(uint8(samples(:)),'int16'),2,[])';
        end
    end
end

