// 不使用环形缓冲池
// 单进程

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
// #include <time.h>
#include <sys/time.h>

#define BUFF_SIZE 1024

asmlinkage int sys_copy(const char* s_file, char* t_file)
{
  // 被复制的文件  只读
  int copy_fd = open(s_file, O_RDONLY);
  if (copy_fd == -1)
  {
    printf("open %s error\n", s_file);
    return -1;
  }

  // 目标路径和文件  没有则创建
  int to_fd = open(t_file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  if (to_fd == -1)
  {
    printf("create %s error\n", t_file);
    return -1;
  }

  char buf[BUFF_SIZE]; //一次读写BUFF_SIZE个字节
  while (1)
  {
    // 读
    int Readsum = read(copy_fd, buf, sizeof(buf));
    if (Readsum == -1)
    {
      printf("read %s error\n", s_file);
      return -1;
    }
    else if (Readsum > 0)
    {
      // 写
      int WriteRes = write(to_fd, buf, Readsum);
      if (WriteRes != Readsum)
      {
        printf("write %s error\n", t_file);
        return -1;
      }
    }
    // 完成
    else if (Readsum == 0)
    {
      printf("copy %s complete\n", s_file);
    }
  }

  return 0;
}

