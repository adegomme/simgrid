/* Copyright (c) 2010-2021. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <simgrid/instr.h>
#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_trace_masterworker, "Messages specific for this example");

struct Task {
  std::string name;
  std::string category;
  double flops;
};

static void master(std::vector<std::string> args)
{
  xbt_assert(args.size() > 4, "The master function expects at least 3 arguments");

  long tasks_count        = std::stol(args[1]);
  double compute_cost     = std::stod(args[2]);
  long communication_cost = std::stol(args[3]);
  size_t workers_count    = args.size() - 4;
  const char* my_hostname = simgrid::s4u::this_actor::get_host()->get_cname();
  auto mailbox            = simgrid::s4u::Mailbox::by_name("master_mailbox");

  XBT_DEBUG("Got %zu workers and %ld tasks to process", workers_count, tasks_count);

  // setting the variable "is_master" (previously declared) to value 1
  TRACE_host_variable_set(my_hostname, "is_master", 1);

  TRACE_mark("msmark", "start_send_tasks");
  for (int i = 0; i < tasks_count; i++) {
    // setting the variable "task_creation" to value i
    TRACE_host_variable_set(my_hostname, "task_creation", i);

    // setting the category of task to "compute"
    Task task = {"task", "compute", compute_cost};
    mailbox->put(new Task(task), communication_cost);
  }
  TRACE_mark("msmark", "finish_send_tasks");

  XBT_DEBUG("All tasks have been dispatched. Request all workers to stop.");
  for (unsigned int i = 0; i < workers_count; i++) {
    Task finalize = {"finalize", "finalize", 0};
    mailbox->put(new Task(finalize), 0);
  }
}

static void worker(std::vector<std::string> args)
{
  xbt_assert(args.size() == 1, "The worker expects no argument");

  const char* my_hostname = simgrid::s4u::this_actor::get_host()->get_cname();
  auto mailbox            = simgrid::s4u::Mailbox::by_name("master_mailbox");

  TRACE_host_variable_set(my_hostname, "is_worker", 1);
  TRACE_host_variable_set(my_hostname, "task_computation", 0);

  while (true) {
    auto task = mailbox->get_unique<Task>();
    if (task->name == "finalize") {
      break;
    }
    // adding the task's cost to the variable "task_computation"
    TRACE_host_variable_add(my_hostname, "task_computation", task->flops);
    simgrid::s4u::this_actor::exec_init(task->flops)
        ->set_name(task->name)
        ->set_tracing_category(task->category)
        ->wait();
  }

  XBT_DEBUG("Exiting now.");
}

int main(int argc, char* argv[])
{
  simgrid::s4u::Engine e(&argc, argv);
  xbt_assert(argc > 2, "Usage: %s platform_file deployment_file\n", argv[0]);

  e.load_platform(argv[1]);

  // declaring user variables
  TRACE_host_variable_declare("is_worker");
  TRACE_host_variable_declare("is_master");
  TRACE_host_variable_declare("task_creation");
  TRACE_host_variable_declare("task_computation");

  // declaring user markers and values
  TRACE_declare_mark("msmark");
  TRACE_declare_mark_value("msmark", "start_send_tasks");
  TRACE_declare_mark_value("msmark", "finish_send_tasks");

  // declaring user categories with RGB colors (values from 0 to 1)
  TRACE_category_with_color("compute", "1 0 0");  // compute is red
  TRACE_category_with_color("finalize", "0 1 0"); // finalize is green
  // categories without user-defined colors receive random colors generated by the tracing system
  TRACE_category("request");
  TRACE_category_with_color("report", nullptr);

  e.register_function("master", &master);
  e.register_function("worker", &worker);
  e.load_deployment(argv[2]);

  e.run();

  XBT_DEBUG("Simulation is over");

  unsigned int cursor;
  xbt_dynar_t categories = TRACE_get_categories();
  if (categories) {
    XBT_INFO("Declared tracing categories:");
    char* category;
    xbt_dynar_foreach (categories, cursor, category) {
      XBT_INFO("%s", category);
    }
    xbt_dynar_free(&categories);
  }

  xbt_dynar_t marks = TRACE_get_marks();
  if (marks) {
    XBT_INFO("Declared marks:");
    char* mark;
    xbt_dynar_foreach (marks, cursor, mark) {
      XBT_INFO("%s", mark);
    }
    xbt_dynar_free(&marks);
  }

  return 0;
}
