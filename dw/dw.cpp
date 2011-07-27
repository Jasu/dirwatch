/**
 * @file
 * dw command line tool
 */

#include <iostream>
#include <boost/filesystem/path.hpp>
#include <dirwatch/dirwatch.h>
#include <unistd.h>
#include <stdlib.h>

class dw_event_handler : public dirwatch::event_handler
{
  public:
    void on_file_contents_changed(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target)
    {
      std::cout << "File contents changed...:"  << target << std::endl;
    }
    void on_node_attributes_changed(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target)
    {
      std::cout << "Node attributes changed.:"  << target << std::endl;
    }
    void on_node_deleted(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target)
    {
      std::cout << "Node was deleted........:"  << target << std::endl;
    }
    void on_node_created(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target)
    {
      std::cout << "Node was created........:"  << target << std::endl;
    }
};

int main(int argc, char ** argv)
{
  boost::filesystem::path p;
  if (argc > 2) 
  {
    std::cerr << "Usage: dw [directory path]" << std::endl
              << "       If directory path is omitted, current working directory is used." << std::endl;
    return -1;
  }
  else if (argc == 2)
  {
    p = argv[1];
  }
  else
  {
    char * wd = (char*)malloc(4096);
    getcwd(wd, 4096);
    p = wd;
    free(wd);
  }
  dw_event_handler handler;
  dirwatch::directory_watcher watcher(p);
  watcher.add_event_handler(&handler);
  std::cout << "Watching " << p << "..." << std::endl;
  watcher.watch();
  return 0;
}

