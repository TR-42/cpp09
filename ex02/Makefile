NAME	:=	PmergeMe

SRCS	:= \
	main.cpp\
	PmergeMe.cpp\

OBJS	:= $(SRCS:.cpp=.o)
DEPS	:= $(OBJS:.o=.d)

override CXXFLAGS	+=	-Wall -Wextra -Werror -MMD -MP -std=c++98

CXX		:=	c++

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

debug: clean_local_obj
	make CXXFLAGS='-DDEBUG -g'
validate: clean_local_obj
	make CXXFLAGS='-DVALIDATE'
faddr: clean_local_obj
	make CXXFLAGS='-g -fsanitize=address'
fleak: clean_local_obj
	make CXXFLAGS='-g -fsanitize=leak'

clean_local_obj:
	rm -f $(OBJS)

clean: clean_local_obj
	rm -f $(DEPS)

fclean: clean
	rm -f $(NAME)

re:	fclean all

-include $(DEPS)

.PHONY:	clean_local_obj
