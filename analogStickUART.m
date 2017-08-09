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
        
    end
end

