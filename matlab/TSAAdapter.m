% TSAAdapter.m
%
% This is a helper module for writing MATLAB applications that interact
% with a Medoc TSA-II via the accompanying hardware TSA-to-USB Adapter.
%
%
% BASIC USAGE:
%
% >> tsa = TSAAdapter()
% >> tsa.connect()
% >> tsa.trigger()  % sends a pulse to the TSA
% >> tsa.check()    % has a pulse been received since last check?
% false
% >> tsa.check()
% false
% >> tsa.check()
% true
% >> tsa.check()
% false
% >> tsa.disconnect()
%
%
%
% COPYRIGHT:
%
% Written by Devon Rueckner for Carissa Cascio
% Vanderbilt University Medical Center
% 2015



classdef TSAAdapter
    properties (SetAccess=protected, Hidden=true)
        % instance variable points to shared port object
        serial_port;
    end;

    methods
        function tsa = TSAAdapter()
            % constructor

            info = instrhwinfo('serial');
            if isempty(info.SerialPorts)
                error('no ports')
            elseif length(info.SerialPorts) > 1
                error('multiple ports')
            end
            % only one port object is ever created
            existingPorts = instrfind;
            if isempty(existingPorts) % if no serial port object has been created, make one
                disp('creating new port');
                tsa.serial_port = serial(info.AvailableSerialPorts, 'BaudRate', 9600);
            elseif length(existingPorts) == 1 % use existing port
                disp('using existing port');
                tsa.serial_port = existingPorts;
            else
                error('multiple port objects exist')
            end
        end

        function trigger(tsa)
            % Send a pulse to the TSA

            tsa.checkPort();
            fwrite(tsa.serial_port, '#')
        end

        function pulse = check(tsa)
            % Check for a pulse from the TSA

            pulse = false;
            tsa.checkPort();
            while tsa.serial_port.BytesAvailable > 0
                result = fread(tsa.serial_port, 1, 'char');
                if result == '#'
                    pulse = true;
                end
            end
        end

        function connect(tsa)
            % Connect to the TSA adapter

            if ~strcmp(tsa.serial_port.Status, 'open')
                disp('opening existing port')
                fopen(tsa.serial_port);
            end
            tsa.checkPort();
            fwrite(tsa.serial_port, '!')
        end

        function disconnect(tsa)
            % Disconnect from the TSA adapter

            tsa.checkPort();
            fclose(tsa.serial_port);
        end
    end

    methods (Hidden=true)
        function checkPort(tsa)
            if ~isvalid(tsa.serial_port)
                error('port is not valid');
            end
            if ~strcmp(tsa.serial_port.Status, 'open')
                error('port is not open');
            end
        end
    end
end
