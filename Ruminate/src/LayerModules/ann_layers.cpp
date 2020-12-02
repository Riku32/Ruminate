#include "ann_layers.hpp"

namespace rum
{
    using ANN = Layer<MLMat>;

    template <typename... Params>
    inline Weight::Weight(uint16_t prev, uint16_t next, RngInit *rng, Params &&... saved_params) : weights(prev, next, saved_params...)
    {
        rng->Generator(weights);
        delete rng;
    }

    inline MLMat &Weight::internal() override
    {
        return weights;
    }
    inline virtual MLMat Weight::ForwardProp(const MLMat &input) override
    {
        //std::cout << "W\n";
        return weights.Dot(input);
    }
    inline virtual MLMat Weight::BackwardProp(MLMat &cost, const std::vector<MLMat> &forwardRes, ANN **layers, size_t index) const override
    {
        //std::cout << "W\n";
        return cost.Dot(forwardRes[index - 1]);
    }

    template <typename... Params>
    inline Hidden::Hidden(uint16_t hidden_nodes, float (*a)(float), float (*ap)(float), Params &&... saved_params) : IActivationFuncs(a, ap), biases(hidden_nodes, 1, saved_params...) {}

    inline MLMat &Hidden::internal() override
    {
        return biases;
    }
    inline virtual MLMat Hidden::ForwardProp(const MLMat &input) override
    {
        //std::cout << "H\n";
        MLMat res(input.SizeX(), input.SizeY());
        for (uint16_t i = 0; i < input.SizeY(); ++i)
        {
            for (uint16_t j = 0; j < input.SizeX(); ++j)
            {
                res.At(j, i) = this->Activation(input.At(j, i) + biases.FastAt(i));
            }
        }

        return res;
    }
    inline virtual MLMat Hidden::BackwardProp(MLMat &cost, const std::vector<MLMat> &forwardRes, ANN **layers, size_t index) const override
    {
        //std::cout << "H\n";
        return cost = cost.Dot(layers[index + 1]->internal()) * forwardRes[index - 1].Transform(ActivationPrime); //TODO: to be optimized
    }

    inline Output(uint16_t hidden_nodes, float (*a)(float), float (*ap)(float)) : Hidden(hidden_nodes, a, ap) {}
    inline virtual MLMat Hidden::BackwardProp(MLMat &cost, const std::vector<MLMat> &forwardRes, ANN **layers, size_t index) const override
    {
        //std::cout << "O\n";
        for (uint32_t i = 0; i < cost.Area(); ++i)
        {
            cost.FastAt(i) *= ActivationPrime(forwardRes[index].FastAt(i));
        }
        return cost;
    }

}; // namespace rum
