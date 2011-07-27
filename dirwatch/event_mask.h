#ifndef DIRWATCH__EVENT_MASK_H
#define DIRWATCH__EVENT_MASK_H

namespace dirwatch
{
  namespace event_mask
  {
    /**
     * @todo More fine-grained control, eg. recursion depth, file/directory for node_.
     */
    const static int file_contents_changed    = 1 << 0;
    const static int node_attributes_changed  = 1 << 1;
    const static int node_deleted             = 1 << 2;
    const static int node_created             = 1 << 3; 
  }
}

#endif

