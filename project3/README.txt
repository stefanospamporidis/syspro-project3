�� ������� ��� �������� ���� ������� ����� �����������. ��� �� ������� �� ��������� �������� �� �� whoServer:
1)	make
2)	./whoServer -q 9000 -s 9500 -w 10 -b 200
���������� �� �� master , � ������ ��������� ��� ������ DiseaseAggregator ��� �������:
1)	make
2)	./master -w 2 -b 50 -s 127.0.0.1 -p 9500 -i ../input_dir
To input_dir ����� �� ������ ��� �������� ��� �� ����� ���� ������� 2, ��������� ��� �� ���� �� ������. ����� ����� � whoClient:
1)	make
2)	./whoClient -q queryFile.txt -w 10 -sp 9000 -sip 127.0.0.1

	���� DiseaseAggregator ��������� �� worker ports ��� �� ������ ����� worker ,���� �� �� port ��� whoServer, �� IP ��� ��� ��� ����� ��� ����� �� ���������. ���������� �� ���� �� worker ports ����� worker ���� � ntohs() �� ������� ����� ��� ��� ���� �� ����� ����. 
	���� whoServer ������� � Circle Buffer � ������ ���� �� ��������� �������� ��� end point port. �� ������� �� ��������� �� ���������� � ��������� ���� ����� ��� �� 1, ��� �� ��������� ����� �� 2. ������� �� �� ��������� ������ ��������� ��� �� ����������� �����������, ������ �� ���� �� ���� ���������� ��� �� worker �������� � receive Info from Workers ������ �������� � query Manager. � ����� ������� �� statistics ��� �� ports ��� worker , ��� �������� �� ��� �����. � ������� ������� �� ��������� ��� �� whoClient, �� ������� ����� worker , ������� �� ����� ������ �������� �� �������� ��� whoServer , � ������ ��� ��������� , ��� �� �� ����� ��� �� ������� ���� ��� whoClient ��� ��� ��������� ��� �����. 
	���� whoClient, ���� readQueries ������ �� ������, ������� ������-������ ��� ������ �� queries , ���� create threads ��� ���� � ������� ���� ����� ���� �� ��� ������ ��� numThreads ��� ������� ��� �� command line ���� ���o�� join ��� �� threads ���� ��� ���� ��� ��� ����. ��� ���������� �� ��������� ���� numThreads > ������� ������� ��� ���� �� �������������� ������� ����� ��������� ��� �� numThreads. 
	���� �� ����� ����������� �� ��� �� ������ ����� ���� ���������� ��������� , mutex , threads, passive endpoints ���� ������� ������������ ��������. �� ����� �� ���� ��� ��� ��������� ��� ����� ��������� ��� �� ��������� ����� �������� ��� ����������� �������. 

