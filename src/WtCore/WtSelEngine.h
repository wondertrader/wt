#pragma once
#include "WtEngine.h"
#include "ISelStraCtx.h"
#include "WtExecuter.h"

#include <unordered_map>
#include <memory>

NS_OTP_BEGIN

typedef enum tagTaskPeriodType
{
	TPT_None,		//���ظ�
	TPT_Daily = 8,	//ÿ��������
	TPT_Weekly,		//ÿ�ܣ������ڼ��յĻ�Ҫ˳��
	TPT_Monthly,	//ÿ�£������ڼ���˳��
	TPT_Yearly		//ÿ�꣬�����ڼ���˳��
}TaskPeriodType;

typedef struct _TaskInfo
{
	uint32_t	_id;
	char		_name[16];		//������
	char		_trdtpl[16];	//������ģ��
	uint32_t	_day;			//���ڣ��������ڱ仯��ÿ��Ϊ0��ÿ��Ϊ0~6����Ӧ���յ�������ÿ��Ϊ1~31��ÿ��Ϊ0101~1231
	uint32_t	_time;			//ʱ�䣬��ȷ������
	bool		_strict_time;	//�Ƿ����ϸ�ʱ�䣬�ϸ�ʱ�伴ֻ��ʱ����ȲŻ�ִ�У������ϸ�ʱ�䣬����ڵ��ڴ���ʱ�䶼��ִ��

	uint64_t	_last_exe_time;	//�ϴ�ִ��ʱ�䣬��ҪΪ�˷�ֹ�ظ�ִ��

	TaskPeriodType	_period;	//��������
} TaskInfo;

typedef std::shared_ptr<TaskInfo> TaskInfoPtr;

typedef boost::shared_ptr<ISelStraCtx> SelContextPtr;
class WtSelRtTicker;


class WtSelEngine : public WtEngine, public IExecuterStub
{
public:
	WtSelEngine();
	~WtSelEngine();

public:
	//////////////////////////////////////////////////////////////////////////
	//WtEngine�ӿ�
	virtual void init(WTSVariant* cfg, IBaseDataMgr* bdMgr, WtDataManager* dataMgr, IHotMgr* hotMgr) override;

	virtual void run(bool bAsync = false) override;

	virtual void on_tick(const char* stdCode, WTSTickData* curTick) override;

	virtual void on_bar(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;

	virtual void handle_push_quote(WTSTickData* newTick, bool isHot) override;

	virtual void on_init() override;

	virtual void on_session_begin() override;

	virtual void on_session_end() override;

	///////////////////////////////////////////////////////////////////////////
	//IExecuterStub �ӿ�
	virtual uint64_t get_real_time() override;
	virtual WTSCommodityInfo* get_comm_info(const char* stdCode) override;
	virtual IHotMgr* IExecuterStub::get_hot_mgr() { return _hot_mgr; }
	virtual uint32_t get_trading_day() { return _cur_tdate; }

public:
	//uint32_t	register_task(const char* name, uint32_t date, uint32_t time, TaskPeriodType period, bool bStrict = true, const char* trdtpl = "CHINA");
	void			addContext(SelContextPtr ctx, uint32_t date, uint32_t time, TaskPeriodType period, bool bStrict = true, const char* trdtpl = "CHINA");

	SelContextPtr	getContext(uint32_t id);

	inline void addExecuter(ExecCmdPtr& executer)
	{
		_exec_mgr.add_executer(executer);
		executer->setStub(this);
	}

	void	on_minute_end(uint32_t uDate, uint32_t uTime);

	void	handle_pos_change(const char* stdCode, double diffQty);

private:
	std::unordered_map<uint32_t, TaskInfoPtr>	_tasks;

	typedef std::unordered_map<uint32_t, SelContextPtr> ContextMap;
	ContextMap		_ctx_map;

	WtExecuterMgr	_exec_mgr;

	bool	_terminated;

	WtSelRtTicker*	_tm_ticker;
	WTSVariant*		_cfg;
};

NS_OTP_END