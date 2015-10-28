��¼ģ��

header files��
	PageHead.h		:	����PageHeadҳͷ��
	RecordManager.h :	����RID��RM_Record, FileHead, RM_FileHandle, PFS, RM_Manager
	rm_filescan.h	:	����RM_Filescan
	
RM Interface:
	RM_Manager		:	����RM���ֵ��½���ɾ�����򿪣��ر��ļ���¼�������ʵ����һ����������
						�������������һ��ʵ��������
						�������£�
							RM_Manager	   (PF_Manager &pfm)								:���캯��,	������ҳʽ�ļ�ϵͳ�Ķ���
							~RM_Manager    ()												:��������
							int CreateFile  (const char *fileName, int recordSize)			:����FileManager::CreateFile�½�һ��ҳ�ļ�fileName
																							 ��ҳ�е����м�¼��С����recordSize.
							int DestroyFile (const char *fileName)							:����FileManager::DestroyFileɾ���ļ���ΪfileName���ļ�
							int OpenFile    (const char *fileName, RM_FileHandle &fileHandle):����FileManagerManager::OpenFile��fileName���ļ�������򿪳ɹ�
																							 ��ͨ��fileHandle��RM��ص��ļ������ﲻ���Ƕ�δ�ͬһ
																							 �ļ��������Ҳ������DestroyFile�����һ�����ڴ򿪵��ļ���
																							 ÿ�δ�ͬһ���ļ��õ��ǲ�ͬ��fileHandle��
							int CloseFile   (RM_FileHandle &fileHandle)						:ͨ��fileHandle���fileID������FileManager:: CloseFile
							
	RM_FileHandle	:	����ʹ��RM Interface�򿪵��ļ��еļ�¼
						��������:
							RM_FileHandle  	()												:���캯��
							~RM_FileHandle  ()												:��������
							int GetRec       (const RID &rid, RM_Record &rec) const			:���������rid����Ҫ��ʾ��������ɹ���rec�д�ż�¼�Ŀ���
							int InsertRec 	(char *pData, RID &rid)							:����һ��pDataָ���������Ϊһ���µļ�¼������ɹ���&rid��
																							 ָ������²����¼��RID
							int DeleteRec 	(RID &rid)										:ɾ���ļ��м�¼ΪRID�ļ�¼�����ҳ��������¼Ϊ�գ�����
																							 FileManager::DisposePage�����ҳ
							int UpdateRec    (RM_Record &rec)								:�������к�rec��صļ�¼�����嶨���RM_Record
																							 
	RM_FileScan		:	�ṩRM�ļ���¼�ļ������ܣ���������������ָ���ļ�������
						��������:
							RM_FileScan  	()												:���캯��
							~RM_FileScan 	()												:��������
							int OpenScan     (const RM_FileHandle &fileHandle,  
											 AttrType      attrType,
											 int           attrLength,
											 int           attrOffset,
											 CompOp        compOp,
											 void          *value,
											 ClientHint    pinHint = NO_HINT)				:��valueΪ��ָ��ʱ����ѯ���м�¼�������Ϊ��ʱ��ָ��value��
																							 ���ڲ�ѯ�Ƚϡ�attrType�Ķ�����database.h��INT��FLOAT��STRING
																							 attrOffsetΪ��ѯ��¼�ĳ�ʼλ��(��ѯĳ���ֶε�ֵ)����ѯ������
																							 �嶨����database.h��
																							 EQ_OP					equal		a = v
																							 LT_OP					less than	a < v
																							 GT_OP					greater than a > v
																							 LE_OP					a <= v
																							 GE_OP					a >= v
																							 NE_OP					a <> v
																							 NO_OP					no comparison value = null
																							 
							int GetNextRec 	(RM_Record &rec)								:��ȡ��һ��record
							int CloseScan 	()												:��ֹ����
							
	RM_Record		:	�ṩ�ļ���¼�Ļ�������
						�������£�
							int Set			(char *pData, int size, RID rid_)				:���ü�¼����
							int GetData		(char *&pData)									:��ȡ��¼����
							int GetRid		(RID &rid)										:��ȡ��¼RID
							
	RID				:	��¼��Identifier
						��������:
							RID				()												:���캯��
							~RID			()												:��������
							int GetPageNum	(int &pageNum) const							:��ȡrecord��ҳ��
							int GetSlotNum	(int &slotNum) const							:��ȡrecord�Ĳ۱��
							int Page		() const										:��ȡrecord��ҳ��
							int Slot		() const   										:��ȡrecord�Ĳ۱��
						
	PageHead		:	����ҳҳͷ��ʽ����ʱ��ʽΪһ��int��84λ��λͼ���
						�������£�
							bool getRecordHead	(int RecordID)								:��ѯRecordID��Ӧ��¼�Ƿ����
							int setRecordHead	(int RecordID, bool IsRecord)				:����RecordID��Ӧ�Ĳ�
	
	FileHead		:	�ļ���һҳ��ʽ���ļ���һҳ�洢
							int recordSize													:��¼����
							int pageNumber													:ҳ�ĸ���
							int recordPerPage												:ÿҳ��¼����
							int	recordNumber												:��¼������

���Բ���							
	main.cpp		:	���Լ�¼����ģ�����ȷ��
						��test_RM_FileScan()������:
						�����½��ļ������ļ���Ȼ�����32byte��С�����ݣ����ݶ���Ϊ��
							struct Rec{
								int b; float f; char a[24];}
						���Բ����¼��ɾ����¼�����¼�¼���ȡ����ֵ�����ض������ļ�¼��

	���Խ������Ԥ��