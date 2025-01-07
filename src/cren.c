// Cren main file

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <args.h>
#include <lib/log.h>
#include <cren.h>

void init_log(args_verbose_t verbose, bool quiet);

int main(int argc, char **argv)
{
  // get args
  args_t *args = args_parse_cmd(argc, argv);
  if (args == NULL)
  {
    puts("Failed to parse arguments");
    return CREN_NOK;
  }

  // check if help
  if (args->help)
  {
    usage(args);
    return CREN_OK;
  }
  // check if version
  if (args->version)
  {
    printf("Cren %s\n", CREN_VERSION);
    return CREN_OK;
  }
  // check if command is unknown
  if (args->cmd == ARGS_CMD_UNKNOWN)
  {
    usage(args);
    return CREN_OK;
  }

  // init log
  init_log(args->verbose, args->quiet);

  // run command
  // TODO: run

  // free args
  args_free(args);

  return CREN_OK;
}

void init_log(args_verbose_t verbose, bool quiet)
{
  log_set_quiet(quiet);

  switch (verbose)
  {
  case VERBOSE_TRACE:
    log_set_level(LOG_TRACE);
    break;
  case VERBOSE_DEBUG:
    log_set_level(LOG_DEBUG);
    break;
  case VERBOSE_INFO:
    log_set_level(LOG_INFO);
    break;
  case VERBOSE_WARN:
    log_set_level(LOG_WARN);
    break;
  case VERBOSE_ERROR:
    log_set_level(LOG_ERROR);
    break;
  case VERBOSE_FATAL:
    log_set_level(LOG_FATAL);
    break;
  default:
    log_set_level(LOG_FATAL);
    log_set_quiet(true);
    break;
  }
}
