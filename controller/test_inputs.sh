#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo "Recompiling controller"

gcc controller.c -o test_controller_args
HELP_CMD=$'Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n'

# Valid options
# ./controller localhost port -o out <file> [arg...]
# ./controller localhost port -o out -log logfile <file> [arg...]
# ./controller localhost port -o out -t seconds <file> [arg...]
# ./controller localhost port -o out -log logfile -t seconds <file> [arg...]

# ./controller localhost port -log logfile <file> [arg...]
# ./controller localhost port -log logfile -t seconds <file> [arg...]

# ./controller localhost port -t seconds <file> [arg...]

# ./controller localhost port <file>
# ./controller localhost port <file> [arg...]

# ./controller localhost port mem
# ./controller localhost port mem [pid]

# ./controller localhost port memkill <percent>

# ./controller --help

echo "Testing all input combinations"

if ./test_controller_args 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with no args passed.${NC}"
else
  echo -e "${RED}Test with no args failed.${NC}"
fi

if ./test_controller_args --help 1> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with --help passed.${NC}"
else
  echo -e "${RED}Test with --help failed.${NC}"
fi

if ./test_controller_args localhost 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with no port passed.${NC}"
else
  echo -e "${RED}Test with no port failed.${NC}"
fi

if ./test_controller_args localhost 8000 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with no options passed.${NC}"
else
  echo -e "${RED}Test with no options failed.${NC}"
fi

if ./test_controller_args localhost 8000 -o 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -o with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -o with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -o outfile 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -o outfile with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -o outfile with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -log logfile 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -log logfile with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -log logfile with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -t 10 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -t seconds with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -t seconds with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -t ten 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -t bad seconds with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -t bad seconds with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -o outfile -log logfile 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -o, -log with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -o, -log with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -o outfile -t 10 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -o, -t with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -o, -t with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -log logfile -t 10 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -log, -t with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -log, -t with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -log logfile -o outfile 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -log, -o with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -log, -o with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -o outfile -log logfile -t 10 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with -log, -o, -t with no <file> passed.${NC}"
else
  echo -e "${RED}Test with -log, -o, -t with no <file> failed.${NC}"
fi

if ./test_controller_args localhost 8000 -o outfile -t 10 -log log file 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with wrong param order passed.${NC}"
else
  echo -e "${RED}Test with wrong param order failed.${NC}"
fi

if ./test_controller_args localhost 8000 mem lol 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with mem bad pid passed.${NC}"
else
  echo -e "${RED}Test with mem bad pid failed.${NC}"
fi

if ./test_controller_args localhost 8000 memkill lol 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with memkill bad % passed.${NC}"
else
  echo -e "${RED}Test with memkill bad % failed.${NC}"
fi

if ./test_controller_args localhost 8000 memkill "42 .5" 2> >(grep -q "${HELP_CMD}"); then
  echo -e "${GREEN}Test with memkill bad % passed.${NC}"
else
  echo -e "${RED}Test with memkill bad % failed.${NC}"
fi
