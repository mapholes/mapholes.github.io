UPDATE manholes SET complaint_type = 'Transformer' WHERE intersection_street1 is null


UPDATE manholes SET complaint_type = 'Gas Pipeline' WHERE intersection_street1 is not null



SELECT * FROM manholes WHERE complaint_type = 'Transformer';


SELECT * FROM manholes WHERE complaint_type = 'Gas Pipeline';

