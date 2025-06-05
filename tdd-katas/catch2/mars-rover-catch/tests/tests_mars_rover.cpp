#include "rover.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <format>
#include <optional>
#include <ranges>
#include <string>

using namespace std;
using namespace TDD;

// Custom string maker for std::pair to work with Catch2
namespace Catch
{
    template <typename T1, typename T2>
    struct StringMaker<std::pair<T1, T2>>
    {
        static std::string convert(const std::pair<T1, T2>& p)
        {
            return "(" + Catch::Detail::stringify(p.first) + ", " + Catch::Detail::stringify(p.second) + ")";
        }
    };
}

TEST_CASE("Map - wraps coordinates")
{
    Map map{10, 20};

    auto [start, expected] = GENERATE(
        std::pair{std::pair{10, 20}, std::pair{0, 0}},
        std::pair{std::pair{15, 25}, std::pair{5, 5}},
        std::pair{std::pair{-1, -1}, std::pair{9, 19}});

    auto [x, y] = start;

    auto result = map.wrap(x, y);
    REQUIRE(result == expected);
}

CATCH_REGISTER_ENUM(Direction, Direction::North, Direction::South, Direction::East, Direction::West);

TEST_CASE("Rover initialized with coord and direction")
{
    // Rover rover(5, 5, Direction::North);
    Rover rover = RoverBuilder{}.build();

    SECTION("reports current postion")
    {
        REQUIRE(rover.get_position() == Position{5, 5, Direction::North});
    }
}

TEST_CASE("Rover - move forward")
{
    auto [start_direction, expected_pos] = GENERATE(
        std::pair{Direction::North, Position{5, 6, Direction::North}},
        std::pair{Direction::South, Position{5, 4, Direction::South}},
        std::pair{Direction::East, Position{6, 5, Direction::East}},
        std::pair{Direction::West, Position{4, 5, Direction::West}});

    DYNAMIC_SECTION("Rover moves forward from " << start_direction)
    {
        // Rover rover(5, 5, start_direction);
        RoverBuilder rover_bld;
        Rover rover = rover_bld.with_position(5, 5, start_direction).build();

        rover.move_forward();

        DYNAMIC_SECTION("Ends on position " << expected_pos)
        {
            REQUIRE(rover.get_position() == expected_pos);
        }
    }
}

TEST_CASE("Rover - move backward")
{
    auto [start_direction, expected_pos] = GENERATE(
        std::pair{Direction::North, Position{5, 4, Direction::North}},
        std::pair{Direction::South, Position{5, 6, Direction::South}},
        std::pair{Direction::East, Position{4, 5, Direction::East}},
        std::pair{Direction::West, Position{6, 5, Direction::West}});

    DYNAMIC_SECTION("Rover moves forward from " << start_direction)
    {
        // Rover rover(5, 5, start_direction);
        RoverBuilder rover_bld;
        Rover rover = rover_bld.with_position(5, 5, start_direction).build();

        rover.move_backward();

        DYNAMIC_SECTION("Ends on position " << expected_pos)
        {
            REQUIRE(rover.get_position() == expected_pos);
        }
    }
}

TEST_CASE("Rover - turn left")
{
    auto [start_direction, expected_direction] = GENERATE(
        std::pair{Direction::North, Direction::West},
        std::pair{Direction::South, Direction::East},
        std::pair{Direction::East, Direction::North},
        std::pair{Direction::West, Direction::South});

    DYNAMIC_SECTION("Rover turns left from " << start_direction)
    {
        // Rover rover(5, 5, start_direction);
        RoverBuilder rover_bld;
        Rover rover = rover_bld.with_position(5, 5, start_direction).build();

        rover.turn_left();

        DYNAMIC_SECTION("Ends facing " << expected_direction)
        {
            REQUIRE(rover.get_position().direction == expected_direction);
        }
    }
}

TEST_CASE("Rover - turn right")
{
    auto [start_direction, expected_direction] = GENERATE(
        std::pair{Direction::North, Direction::East},
        std::pair{Direction::South, Direction::West},
        std::pair{Direction::East, Direction::South},
        std::pair{Direction::West, Direction::North});

    DYNAMIC_SECTION("Rover turns right from " << start_direction)
    {
        // Rover rover(5, 5, start_direction);
        RoverBuilder rover_bld;
        Rover rover = rover_bld.with_position(5, 5, start_direction).build();

        rover.turn_right();

        DYNAMIC_SECTION("Ends facing " << expected_direction)
        {
            REQUIRE(rover.get_position().direction == expected_direction);
        }
    }
}

TEST_CASE("Rover - executes list of commands")
{
    // Rover rover(5, 5, Direction::East);
    RoverBuilder rover_bld;
    Rover rover = rover_bld.with_position(5, 5, Direction::East).build();

    SECTION("Uppercase set of commands")
    {
        rover.execute("FFLBR");

        REQUIRE(rover.get_position() == Position(7, 4, Direction::East));
    }

    SECTION("set of commands - case insensitive")
    {
        rover.execute("FfLBr");

        REQUIRE(rover.get_position() == Position(7, 4, Direction::East));
    }

    SECTION("Unknown command")
    {
        SECTION("throws exception")
        {
            REQUIRE_THROWS_AS(rover.execute("FFxFFF"), UnknownCommandException);
        }

        SECTION("rover stops and reports position")
        {
            try
            {
                rover.execute("FFxFFF");
            }
            catch (...)
            {
            }

            REQUIRE(rover.get_position() == Position(7, 5, Direction::East));
        }
    }
}

TEST_CASE("Rover - wrapping coordinates")
{
    RoverBuilder rover_bld;
    
    Map mars_map{10, 10};
    rover_bld.with_map(mars_map);


    SECTION("Rover wraps coordinates when moving north")
    {
        //Rover rover(5, 9, Direction::North, mars_map);
        Rover rover = rover_bld.with_position(5, 9, Direction::North).build();

        rover.move_forward();

        REQUIRE(rover.get_position() == Position(5, 0, Direction::North));
    }

    SECTION("Rover wraps coordinates when moving south")
    {
        //Rover rover(5, 0, Direction::South, mars_map);
        Rover rover = rover_bld.with_position(5, 0, Direction::South).build();

        rover.move_forward();

        REQUIRE(rover.get_position() == Position(5, 9, Direction::South));
    }

    SECTION("Rover wraps coordinates when moving east")
    {
        //Rover rover(9, 5, Direction::East, mars_map);
        Rover rover = rover_bld.with_position(9, 5, Direction::East).build();

        rover.move_forward();

        REQUIRE(rover.get_position() == Position(0, 5, Direction::East));
    }

    SECTION("Rover wraps coordinates when moving west")
    {
        //Rover rover(5, 9, Direction::North, mars_map);
        Rover rover = rover_bld.with_position(5, 9, Direction::North).build();

        rover.move_forward();

        REQUIRE(rover.get_position() == Position(5, 0, Direction::North));
    }
}

class ObstacleDetectorStub : public ObstacleDetector
{
public:
    bool is_obstacle_for(size_t x, size_t y) override
    {
        if (x == 5 && y == 8)
            return true;
        return false;
    };
};

TEST_CASE("Rover - obstacle detection")
{
    Map mars_map{10, 10};
    auto obstacle_detector = std::make_unique<ObstacleDetectorStub>();
    
    RoverBuilder rover_bld;
    rover_bld.with_position(5, 5, Direction::North);
    rover_bld.with_map(mars_map);
    rover_bld.with_obstacle_detector(std::move(obstacle_detector));

    Rover rover = rover_bld.build();

    SECTION("obstacle detected")
    {
        REQUIRE_THROWS_AS(rover.execute("FFFFFFF"), ObstacleDetectedException);
    }

    SECTION("obstacle detected")
    {
        REQUIRE_THROWS(rover.execute("FFFFFFF"));

        REQUIRE(rover.get_position() == Position{5, 7, Direction::North});
    }
}

struct MockObstacleDetector : public ObstacleDetector
{
    MAKE_MOCK2(is_obstacle_for, bool(size_t, size_t));
};

TEST_CASE("Rover - obstacle detection with mock")
{
    using ::trompeloeil::_;

    Map mars_map{10, 10};
    auto obstacle_detector = std::make_unique<MockObstacleDetector>();
    auto& mock_obstacle_detector = *obstacle_detector;
    
    REQUIRE_CALL(mock_obstacle_detector, is_obstacle_for(_, _)).TIMES(2).RETURN(false);
    REQUIRE_CALL(mock_obstacle_detector, is_obstacle_for(5, 8)).RETURN(true);

    RoverBuilder rover_bld;
    rover_bld.with_position(5, 5, Direction::North);
    rover_bld.with_map(mars_map);
    rover_bld.with_obstacle_detector(std::move(obstacle_detector));

    Rover rover = rover_bld.build();

    SECTION("obstacle detected")
    {
        REQUIRE_THROWS_AS(rover.execute("FFFFFFF"), ObstacleDetectedException);
    }

    SECTION("obstacle detected")
    {
        REQUIRE_THROWS(rover.execute("FFFFFFF"));

        REQUIRE(rover.get_position() == Position{5, 7, Direction::North});
    }
}