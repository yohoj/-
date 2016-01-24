#include "sconfig.h"
#include "ihome.h"
#include "md5.h"
SConfig::SConfig()
{
    log.Log("SConfig:SConfig",COMMON,"初始化");
    CreateNode();
    ParseConfigFile(ConfigPath);
    log.Log("SConfig:SConfig",COMMON,"初始化完成");
    //display();
}

SConfig::~SConfig()
{
       SaveConfig(ConfigPath);
      DestroyNode();
      log.Log("SConfig:～SConfig",COMMON,"SConfig结束");
}

int SConfig::CreateNode()
{
     log.Log("SConfig::CreateNode",COMMON,"创建链表");
    head =  new ConfigNode;
    if(head == NULL)
    {
        log.Log("SConfig::CreateNode",ERROR,"创建链表失败：内存申请失败");
        return -1;
    }
    strcpy(head->key,"\0");
    strcpy(head->value,"\0");
    head->next = NULL;
    tail = head;
    log.Log("SConfig::CreateNode",COMMON,"成功创建链表");
    return 0;
}

int SConfig::AddList(const char *key,const char *value)
{
     log.Log("SConfig:AddList",COMMON,"添加节点");
    ConfigNode *pnew = new  ConfigNode;
    if(pnew == NULL)
    {
         log.Log("SConfig:AddList",ERROR,"添加节点失败：内存申请失败");
        return -1;
    }
    strcpy(pnew->key,key);
    strcpy(pnew->value,value);
    pnew->next = NULL;
    tail->next = pnew;
    tail = pnew;
    log.Log("SConfig:AddList",COMMON,"添加节点成功");
    return 0;

}
int SConfig::display()
{
     log.Log("SConfig:display",COMMON,"开始遍历链表");
    ConfigNode *p = head->next;
    while(p)
    {
        cout<<p->key<<"="<<p->value<<endl;
        p = p->next;
    }
     log.Log("SConfig:display",COMMON,"遍历链表完成");
    return 0;
}
int SConfig::FreeList()
{
    log.Log("SConfig:FreeList",COMMON,"开始删除节点");
    ConfigNode *p = head->next;
    ConfigNode *q = head;
    while(p)
    {
        q->next = p->next;
        delete p;
        p = q->next;
    }
    log.Log("SConfig:FreeList",COMMON,"删除节点完成");
    return 0;
}
int SConfig:: DestroyNode()
{
    log.Log("SConfig:DestroyNode",COMMON,"开始销毁链表");
    FreeList();
    delete head;
    log.Log("SConfig:DestroyNode",COMMON,"销毁链表完成");
     return 0;
}
//1. 解析配置文件函数
bool SConfig::ParseConfigFile(const char *path)
{
    log.Log("SConfig::ParseConfigFile",COMMON,"开始解析配置文件");
    int fd = open(path,O_RDONLY);
    if(fd == -1)
    {
        log.Log("SConfig::ParseConfigFile",ERROR,"open error");
        return false;
    }
    char buf[32]={0};
    char key[32]={0};
    char value[32]={0};
    int len;
    int flag = 0;
    int i = 0;
    while(1)
    {
        flag = 0;
        len = read(fd,&buf[i],1);
        if(len <= 0)
        {
            break;
        }
        if(buf[i] == '=')
        {
            strcpy(key,buf);
            key[i] = '\0';
            memset(buf,0,sizeof(buf));
            i = 0;
            flag = 1;
        }
        else if(buf[i] == '\n')
        {
            strcpy(value,buf);
            value[i] = '\0';
            memset(buf,0,sizeof(buf));
            i = 0;
            flag = 1;
        }
        if(key[0] != '\0' && value[0] != '\0')
        {
            AddList(key,value);
            memset(key,0,sizeof(key));
            memset(value,0,sizeof(value));
        }
        if(flag == 0)
        {
            i++;
        }
    }
    close(fd);
    log.Log("SConfig::ParseConfigFile",COMMON,"完成配置文件解析");
    return true;
}
//2. 获取某个关键字的值,当没有该关键字时，返回空
const char *SConfig::GetKeyValue(const char *key)
{
    log.Log("SConfig::GetKeyValue",COMMON,"开始获取关键字的值");
    ConfigNode *p = head->next;
    while(p)
    {
        if(strcmp(key,p->key) == 0)
        {
             log.Log("SConfig::GetKeyValue",COMMON,"完成关键字的值获取");
            return p->value;
        }
        p = p->next;
    }
     log.Log("SConfig::GetKeyValue",WARNING,"不存在该关键字");
    return " ";
}
//3. 修改配置项
bool SConfig::ModifyKey(const char *key, const char *value)
{
    log.Log("SConfig::ModifyKey",COMMON,"修改配置项");
    ConfigNode *p = head->next;
    char passwd[32];
    while(p)
    {
        if(strncmp(key,p->key,strlen(p->key)) == 0)
        {
            memset(p->value,0,sizeof(p->value));
            if(strcmp(key,"passwd")==0)
            {
                 MD5Encode((void *)value,strlen(value),passwd);
                 strcpy(p->value,passwd);
            }
            else
            {
                strcpy(p->value,value);
            }
            log.Log("SConfig::ModifyKey",COMMON,"修改配置项成功");
            return true;
        }
        p = p->next;
    }
    log.Log("SConfig::ModifyKey",WARNING,"修改配置项失败");
    return false;
}
//4. 保存配置
bool SConfig::SaveConfig(const char *path)
{
     log.Log(" SConfig::SaveConfig",COMMON,"开始保存配置");
    int fd = open(path,O_WRONLY|O_TRUNC);
    if(fd == -1)
    {
         log.Log(" SConfig::SaveConfig",ERROR,"打开文件失败");
        return false;
    }
   ConfigNode *p = head->next;
   while(p)
   {
       write(fd,p->key,strlen(p->key));
       write(fd,"=",strlen("="));
       write(fd,p->value,strlen(p->value));
       write(fd,"\n",strlen("\n"));
       p = p->next;
   }
   log.Log(" SConfig::SaveConfig",COMMON,"成功保存配置");
   return true;
}
