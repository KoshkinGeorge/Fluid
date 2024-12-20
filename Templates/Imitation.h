#pragma once

#include <bits/stdc++.h>
#include <random>

#include "./Types.h"

using namespace std;

// height = 0 or width = 0 means that size is calculated dynamically
template <typename First, typename Second, typename Third, size_t height = 0, size_t width = 0>
class Imitation
{
private:
    template <typename T>
    using Container = std::conditional_t<((height * width) != 0), std::array<std::array<T, width>, height>, std::vector<std::vector<T>>>;

private:
    // private fields
    static constexpr size_t Ticks = 1'000'000;
    static constexpr std::array<std::pair<int, int>, 4> deltas
    {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    Container<char> field;
    Container<int> dirs;

    Double rho[256];
    Double g = 0.1;

    Container<First> p, old_p;

private:
    // private types

    template <typename Type>
    struct VectorField {
        Container<std::array<Type, deltas.size()>> v;

        VectorField() = default;

        VectorField(size_t rows, size_t cols) requires((height * width) == 0):
        v(decltype(v)(rows, typename decltype(v)::value_type(cols))) {}

        Type &add(int x, int y, int dx, int dy, Type dv) {
            return (get(x, y, dx, dy) += dv);
        }

        Type &get(int x, int y, int dx, int dy) {
            size_t i = std::ranges::find(deltas, std::pair(dx, dy))
            - deltas.begin();
            assert(i < deltas.size());
            return v[x][y][i];
        }

        void clear()
        {
            for (auto &row : v)
            {
                for (auto &arr : row)
                {
                    arr = {};
                }
            }
        }
    };

    struct ParticleParams
    {
        Imitation *imitation;
        char type;
        First cur_p;
        std::array<First, deltas.size()> v;

        ParticleParams(Imitation *ins): imitation(ins) {}

        void swap_with(int x, int y) 
        {
            std::swap(imitation->field[x][y], type);
            std::swap(imitation->p[x][y], cur_p);
            for (size_t i = 0; i < v.size(); i++)
            {
                swap(v[i], imitation->velocity.v[x][y][i]);
            }
        }
    };

private:

    VectorField<Second> velocity;
    VectorField<Third> velocity_flow;
    Container<int> last_use;
    int UT = 0;

    // За счёт улучшенного рандома программа ускоряется, не теряя корректности
    random_device rd;
    std::mt19937 engine{rd()};
    uniform_real_distribution<Double> dist{0, 1};

private:
    // private methods

    std::tuple<Double, bool, std::pair<int, int>>
    propagate_flow(int x, int y, Double lim)
    {
        last_use[x][y] = UT - 1;
        Double ret = 0;
        for (auto [dx, dy] : deltas)
        {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT)
            {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);

                if (flow == cap)
                {
                    continue;
                }
                // assert(v >= velocity_flow.get(x, y, dx, dy));
                auto vp = std::min(lim, static_cast<Double>(cap - flow));

                if (last_use[nx][ny] == UT - 1) 
                {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;
                    // cerr << x << " " << y << " -> " << nx << " " << ny << " " << vp << " / " << lim << "\n";
                    return {vp, 1, {nx, ny}};
                }

                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop)
                {
                    velocity_flow.add(x, y, dx, dy, t);
                    last_use[x][y] = UT;
                    // cerr << x << " " << y << " -> " << nx << " " << ny << " " << t << " / " << lim << "\n";
                    return {t, prop && end != std::pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, 0, {0, 0}};
    }

    Double random01()
    {
        return dist(engine);
    }

    void propagate_stop(int x, int y, bool force = false)
    {
        if (!force) {
            bool stop = true;
            for (auto [dx, dy] : deltas) {
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0) {
                    stop = false;
                    break;
                }
            }
            if (!stop) {
                return;
            }
        }
        last_use[x][y] = UT;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0) {
                continue;
            }
            propagate_stop(nx, ny);
        }
    }

    Double move_prob(int x, int y)
    {
        Double sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            if (v <= 0) {
                continue;
            }
            sum += v;
        }
        return sum;
    }

    bool propagate_move(int x, int y, bool is_first)
    {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        do
        {
            std::array<Double, deltas.size()> tres;
            Double sum = 0;
            for (size_t i = 0; i < deltas.size(); ++i)
            {
                auto [dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] == '#' || last_use[nx][ny] == UT)
                {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if (v < 0)
                {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if (sum == 0)
            {
                break;
            }

            Double p = random01() * sum;
            size_t d = std::ranges::upper_bound(tres, p) - tres.begin();

            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            assert(velocity.get(x, y, dx, dy) > 0 && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        } while (!ret);
        last_use[x][y] = UT;
        for (size_t i = 0; i < deltas.size(); ++i)
        {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0)
            {
                propagate_stop(nx, ny);
            }
        }
        if (ret)
        {
            if (!is_first)
            {
                ParticleParams pp(this);
                pp.swap_with(x, y);
                pp.swap_with(nx, ny);
                pp.swap_with(x, y);
            }
        }
        return ret;
    }

public:
    Imitation(const std::vector<std::vector<char>> &_field)
    {
        for (size_t i = 0; i < field.size(); i++)
        {
            for (size_t j = 0; j < field.begin()->size(); j++)
            {
                field[i][j] = _field[i][j];
            }
        }
    }

    // Due to this requires exspression, we won`t have any ambiguous calls
    Imitation(std::vector<std::vector<char>> &&_field) requires ((height * width) == 0):
    field(std::move(_field)),
    // resize all vector to fit everything
    dirs(Container<int>(field.size(), typename Container<int>::value_type(field.begin()->size()))),
    p(Container<First>(field.size(), typename Container<First>::value_type(field.begin()->size()))),
    old_p(p),
    velocity(field.size(), field.begin()->size()),
    velocity_flow(field.size(), field.begin()->size()),
    last_use(dirs){}

    void imitate()
    {
        rho[' '] = 0.01;
        rho['.'] = 1000;

        for (size_t x = 0; x < field.size(); ++x)
        {
            for (size_t y = 0; y < field.begin()->size(); ++y)
            {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas)
                {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }

        for (size_t i = 0; i < Ticks; ++i)
        {
            Double total_delta_p = 0;
            // Apply external forces
            for (size_t x = 0; x < field.size(); ++x)
            {
                for (size_t y = 0; y < field.begin()->size(); ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    if (field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, g);
                }
            }

            // Apply forces from p
            std::copy(p.begin(), p.end(), old_p.begin());
            for (size_t x = 0; x < field.size(); ++x)
            {
                for (size_t y = 0; y < field.begin()->size(); ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas)
                    {
                        int nx = x + dx, ny = y + dy;
                        if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y]) {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = delta_p;
                            auto &contr = velocity.get(nx, ny, -dx, -dy);
                            if (contr * rho[(int) field[nx][ny]] >= force)
                            {
                                contr -= force / rho[(int) field[nx][ny]];
                                continue;
                            }
                            force -= contr * rho[(int) field[nx][ny]];
                            contr = 0;
                            velocity.add(x, y, dx, dy, force / rho[(int) field[x][y]]);
                            p[x][y] -= force / dirs[x][y];
                            total_delta_p -= force / dirs[x][y];
                        }
                    }
                }
            }

            // Make flow from velocities
            velocity_flow.clear();
            bool prop = false;
            do
            {
                UT += 2;
                prop = 0;
                for (size_t x = 0; x < field.size(); ++x)
                {
                    for (size_t y = 0; y < field.begin()->size(); ++y)
                    {
                        if (field[x][y] != '#' && last_use[x][y] != UT)
                        {
                            auto [t, local_prop, _] = propagate_flow(x, y, 1);
                            if (t > 0)
                            {
                                prop = 1;
                            }
                        }
                    }
                }
            } while (prop);

            // Recalculate p with kinetic energy
            for (size_t x = 0; x < field.size(); ++x)
            {
                for (size_t y = 0; y < field.begin()->size(); ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas)
                    {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = velocity_flow.get(x, y, dx, dy);
                        if (old_v > 0)
                        {
                            assert(new_v <= old_v);
                            velocity.get(x, y, dx, dy) = new_v;
                            auto force = (old_v - new_v) * rho[(int) field[x][y]];
                            if (field[x][y] == '.')
                                force *= 0.8;
                            if (field[x + dx][y + dy] == '#')
                            {
                                p[x][y] += force / dirs[x][y];
                                total_delta_p += force / dirs[x][y];
                            } 
                            else
                            {
                                p[x + dx][y + dy] += force / dirs[x + dx][y + dy];
                                total_delta_p += force / dirs[x + dx][y + dy];
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for (size_t x = 0; x < field.size(); ++x)
            {
                for (size_t y = 0; y < field.begin()->size(); ++y) {
                    if (field[x][y] != '#' && last_use[x][y] != UT)
                    {
                        if (random01() < move_prob(x, y))
                        {
                            prop = true;
                            propagate_move(x, y, true);
                        }
                        else
                        {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }

            if (prop)
            {
                std::cout << "Tick " << i << ":\n";
                for (size_t x = 0; x < field.size(); ++x)
                {
                    for (size_t y = 0; y < field.begin()->size(); ++y)
                    {
                        std::cout << field[x][y];
                    }
                    std::cout << '\n';
                }
            }
        }
    }
};